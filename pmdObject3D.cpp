#include "pmdObject3D.h"

ComPtr<ID3D12RootSignature> PMDobject::_rootsignature;
ComPtr<ID3D12PipelineState> PMDobject::_pipelinestate;

bool PMDobject::StaticInitialize(ID3D12Device* device, SIZE ret)
{
	//assert(!PMDobject::device);

	assert(device);

	PMDobject::device = device;

	InitializeGraphicsPipeline();

    return true;
}

bool PMDobject::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	result = D3DCompileFromFile(//VS
		L"Resources/shaders/BasicVertexShader.hlsl",//シェーダー名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS", "vs_5_0",//関数、対象シェーダー
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("ファイルが見当たりません");
			return 0;//exit()
		}
		else {
			string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//データを表示
		}
	}

	result = D3DCompileFromFile(//PS
		L"Resources/shaders/BasicPixelShader.hlsl",//シェーダー名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS", "ps_5_0",//関数、対象シェーダー
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("ファイルが見当たりません");
			return 0;//exit()
		}
		else {
			string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());//データを表示
		}
	}
#pragma endregion

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{//頂点座標
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//法線
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//uv
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//ボーン番号
		"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//ウェイト
		"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{//輪郭線フラグ
		"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//サンプルマスク,ラスタライザー
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//デフォルトのサンプルマスクを表す定数
	gpipeline.RasterizerState.MultisampleEnable = false;//アンチエイリアス（まだ使わない）
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//中身を塗りつぶす
	gpipeline.RasterizerState.DepthClipEnable = true;//深度方向のクリッピング有効

	//レンダターゲットの設定
	gpipeline.NumRenderTargets = 1;//今は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;//0～1に正規化されたRGBA

	//深度ステンシル
	gpipeline.DepthStencilState.DepthEnable = true;//使う
	gpipeline.DepthStencilState.StencilEnable = false;//
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込む
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さい方を採用
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//ブレンドステートの設定
	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlenddesc = {};
	renderTargetBlenddesc.BlendEnable = false;
	renderTargetBlenddesc.LogicOpEnable = false;
	renderTargetBlenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	gpipeline.BlendState.RenderTarget[0] = renderTargetBlenddesc;

	//入力レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;//レイアウトの先頭アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout);//レイアウト配列の要素
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//カットなし
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成

	//アンチエイリアシングのためのサンプル数設定
	gpipeline.SampleDesc.Count = 1;
	gpipeline.SampleDesc.Quality = 0;

	//ルートシグネチャ
	//ID3D12RootSignature* rootsignature;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//デスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE descTblRange[3] = {};
	descTblRange[0].NumDescriptors = 1;//定数1つ目
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//種別(定数)
	descTblRange[0].BaseShaderRegister = 0;//０番スロットから
	descTblRange[0].OffsetInDescriptorsFromTableStart
		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descTblRange[1].NumDescriptors = 1;//定数2つ目
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//種別(定数)
	descTblRange[1].BaseShaderRegister = 1;//1番スロットから
	descTblRange[1].OffsetInDescriptorsFromTableStart
		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descTblRange[2].NumDescriptors = 3;//テクスチャ2つ
	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//種別(テクスチャ)
	descTblRange[2].BaseShaderRegister = 0;//０番スロットから
	descTblRange[2].OffsetInDescriptorsFromTableStart
		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメーター
	D3D12_ROOT_PARAMETER rootparam[2] = {};
	//rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	//rootparam[0].DescriptorTable.NumDescriptorRanges = 1;
	//rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	//本来のやり方ではないよー
	rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootparam[0].Descriptor.RegisterSpace = 0;
	rootparam[0].Descriptor.ShaderRegister = 0;
	rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
	rootparam[1].DescriptorTable.NumDescriptorRanges = 2;
	rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	rootSignatureDesc.pParameters = rootparam;//ルートパラメーターの先頭アドレス
	rootSignatureDesc.NumParameters = 2;//ルートパラメーター数

	//サンプラー
	D3D12_STATIC_SAMPLER_DESC sampleDesc = {};
	sampleDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横方向の繰り返し
	sampleDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦方向の繰り返し
	sampleDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行きの繰り返し
	sampleDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーは黒
	sampleDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;//線形補間
	sampleDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
	sampleDesc.MinLOD = 0.0f;//ミップマップ最小値
	sampleDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダから見える
	sampleDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//リサンプリングしない

	rootSignatureDesc.pStaticSamplers = &sampleDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	if (FAILED(result)) {
		return result;
	}

	result = device->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&_rootsignature));
	if (FAILED(result)) {
		return result;
	}

	//rootSigBlob->Release();
	gpipeline.pRootSignature = _rootsignature.Get();

	//グラフィックスパイプラインステートオブジェクトの生成
	//ID3D12PipelineState* _pipelinestate = nullptr;
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));
	if (FAILED(result)) {
		return result;
	}

    return true;
}

PMDobject* PMDobject::Create()
{
    //モデルのインスタンスを作成
    PMDobject* pmdObject = new PMDobject();
    if (pmdObject == nullptr) {
        return nullptr;
    }

    //初期化
    if (!pmdObject->Initialize()) {
        delete pmdObject;
        assert(0);
        return nullptr;
    }

    return pmdObject;
}

void PMDobject::SetModel(PMDmodel* model)
{
	this->model = model;
	model->Initialize();
}

bool PMDobject::Initialize()
{
	assert(device);

	HRESULT result;

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&PMDconstBuffB0));

    return true;
}

void PMDobject::Update()
{
	//HRESULT result;
	//XMMATRIX matScale, matRot, matTrans;

	//// スケール、回転、平行移動行列の計算
	//matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//matRot = XMMatrixIdentity();
	//matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	//matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	//matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	//matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//// ワールド行列の合成
	//matWorld = XMMatrixIdentity(); // 変形をリセット
	//matWorld *= matScale; // ワールド行列にスケーリングを反映
	//matWorld *= matRot; // ワールド行列に回転を反映
	//matWorld *= matTrans; // ワールド行列に平行移動を反映

	//const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	//const XMFLOAT3& cameraPos = camera->GetEye();

	//// 定数バッファへデータ転送(OBJ)
	//ConstBufferDataB0* constMap = nullptr;
	//result = PMDconstBuffB0->Map(0, nullptr, (void**)&constMap);
	//if (FAILED(result)) {
	//	assert(0);
	//}

	////constMap->mat = matWorld * matView * matProjection;	// 行列の合成
	//constMap->viewproj = matViewProjection;
	//constMap->world = matWorld;
	//constMap->cameraPos = cameraPos;
	//PMDconstBuffB0->Unmap(0, nullptr);

	model->Update();
}

void PMDobject::Draw()
{
	// nullptrチェック
	assert(device);
	assert(cmdList);

	cmdList->IASetVertexBuffers(0, 1, &model->VbView());
	cmdList->IASetIndexBuffer(&model->IbView());

	// パイプラインステートの設定
	cmdList->SetPipelineState(_pipelinestate.Get());

	cmdList->SetGraphicsRootSignature(_rootsignature.Get());

	cmdList->SetGraphicsRootConstantBufferView(0, PMDconstBuffB0->GetGPUVirtualAddress());//本来のやり方ではないよー

	model->Draw(PMDobject::cmdList);

	auto materialH = model->DescHeap()->GetGPUDescriptorHandleForHeapStart();
	unsigned int idxOffset = 0;
	auto cbvsrvIncSize = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;
	for (auto& m : model->Materials()) {

		cmdList->SetGraphicsRootDescriptorTable(2, materialH);
		cmdList->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);

		//ヒープポインターとインデックスを次に進める
		materialH.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}

}
