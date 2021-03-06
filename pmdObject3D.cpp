#include "pmdObject3D.h"

ComPtr<ID3D12RootSignature> PMDobject::_rootsignature;
ComPtr<ID3D12PipelineState> PMDobject::_pipelinestate;

void PMDobject::Update()
{
	HRESULT result;
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

	//// 定数バッファへデータ転送(PMD)
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
	//PMDconstBuffB0->SetName(L"SSSSSS");

	//model->Update();
	_mappedSceneData = nullptr;//マップ先を示すポインタ
	result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//マップ

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

	_mappedSceneData->viewproj = matViewProjection;
	_mappedSceneData->cameraPos = cameraPos;

	_sceneConstBuff->Unmap(0, nullptr);
}

void PMDobject::Draw()
{
	// パイプラインステートの設定
	cmdList->SetPipelineState(_pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(_rootsignature.Get());

	//現在のシーン(ビュープロジェクション)をセット
	ID3D12DescriptorHeap* sceneheaps[] = { _sceneDescHeap.Get() };
	cmdList->SetDescriptorHeaps(1, sceneheaps);
	cmdList->SetGraphicsRootDescriptorTable(0, _sceneDescHeap->GetGPUDescriptorHandleForHeapStart());
}

PMDobject::PMDobject()
{
	assert(SUCCEEDED(CreateRootSignaturePMD()));
	assert(SUCCEEDED(CreateGraphicsPipelinePMD()));
	assert(SUCCEEDED(CreateSceneView()));
	assert(SUCCEEDED(CreateDescHeap()));
}

PMDobject::~PMDobject()
{
}

HRESULT PMDobject::CreateGraphicsPipelinePMD()
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
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0〜1に正規化されたRGBA

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

	//rootSigBlob->Release();
	gpipeline.pRootSignature = _rootsignature.Get();

	//グラフィックスパイプラインステートオブジェクトの生成
	//ID3D12PipelineState* _pipelinestate = nullptr;
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));
	if (FAILED(result)) { return result; }

	return S_OK;
}

HRESULT PMDobject::CreateRootSignaturePMD()
{
	//レンジ
	CD3DX12_DESCRIPTOR_RANGE  descTblRanges[4] = {};//テクスチャと定数の２つ
	descTblRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//定数[b0](ビュープロジェクション用)
	descTblRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);//定数[b1](ワールド、ボーン用)
	descTblRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);//定数[b2](マテリアル用)
	descTblRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);//テクスチャ４つ(基本とsphとspaとトゥーン)

	//ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootParams[3] = {};
	rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

	rootParams[0].InitAsDescriptorTable(1, &descTblRanges[0]);//ビュープロジェクション変換
	rootParams[1].InitAsDescriptorTable(1, &descTblRanges[1]);//ワールド・ボーン変換
	rootParams[2].InitAsDescriptorTable(2, &descTblRanges[2]);//マテリアル周り

	CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
	samplerDescs[0].Init(0);
	samplerDescs[1].Init(1, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(3, rootParams, 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, &errorBlob);
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return result;
	}
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(_rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return result;
	}
	return result;
}

HRESULT PMDobject::CreateSceneView()
{
	HRESULT result;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0_1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_sceneConstBuff.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return result;
	}

	_mappedSceneData = nullptr;//マップ先を示すポインタ
	result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//マップ

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

	_mappedSceneData->viewproj = matViewProjection;
	_mappedSceneData->cameraPos = cameraPos;

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NodeMask = 0;//マスクは0
	descHeapDesc.NumDescriptors = 1;//
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//デスクリプタヒープ種別
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(_sceneDescHeap.ReleaseAndGetAddressOf()));//生成

	////デスクリプタの先頭ハンドルを取得しておく
	auto heapHandle = _sceneDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _sceneConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = _sceneConstBuff->GetDesc().Width;
	//定数バッファビューの作成
	device->CreateConstantBufferView(&cbvDesc, heapHandle);
	return result;
}

HRESULT PMDobject::CreateDescHeap() {
	//ディスクリプタヒープ生成(汎用)
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//どんなビューを作るのか()
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1024;//全体のヒープ領域数
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//特に指定なし

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	auto result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_DescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}	
	//result = _swapchain->GetDesc(&swcDesc);
	//if (FAILED(result)) {
	//	assert(0);
	//	return result;
	//}

	//先頭アドレスの設定
	//heapHandle = _basicDescHeap->GetCPUDescriptorHandleForHeapStart();
	//_backBuffer.resize(swcDesc.BufferCount);

	////SRGBレンダターゲットビュー設定
	//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//for (int i = 0; i < swcDesc.BufferCount; ++i) {
	//	result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffer[i]));
	//	if (FAILED(result)) {
	//		assert(0);
	//		return result;
	//	}

	//	_backBuffer[i]->SetName(L"buffer");

	//	//rtvDesc.Format = _backBuffer[i]->GetDesc().Format;
	//	//レンダターゲットビューの生成
	//	_dev->CreateRenderTargetView(_backBuffer[i].Get(), &rtvDesc, handle);
	//	//ポインターをずらす
	//	handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//}

	return result;
}
