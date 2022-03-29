#include "object3D.h"


// デバイス
ID3D12Device* Object3Ds::device;
// コマンドリスト
ID3D12GraphicsCommandList* Object3Ds::cmdList;

ComPtr<ID3D12RootSignature> Object3Ds::_rootsignature;
ComPtr<ID3D12PipelineState> Object3Ds::_pipelinestate;

XMMATRIX Object3Ds::matView{};
XMMATRIX Object3Ds::matProjection{};
XMFLOAT3 Object3Ds::eye = { 0, 0, -10.0f };
XMFLOAT3 Object3Ds::target = { 0, 0, 0 };
XMFLOAT3 Object3Ds::up = { 0, 1, 0 };

Camera* Object3Ds::camera = nullptr;

bool Object3Ds::StaticInitialize(ID3D12Device* device, SIZE ret)
{
	// 再初期化チェック
	assert(!Object3Ds::device);

	// nullptrチェック
	assert(device);

	Object3Ds::device = device;

	//パイプライン生成
	InitializeGraphicsPipeline();

	//カメラ初期化
	InitalizeCamera(ret.cx, ret.cy);

	return true;
}

//void Object3Ds::PreDraw(ID3D12GraphicsCommandList* cmdList)
//{
//	// PreDrawとPostDrawがペアで呼ばれていなければエラー
//	assert(Object3Ds::cmdList == nullptr);
//
//	// コマンドリストをセット
//	Object3Ds::cmdList = cmdList;
//
//	// プリミティブ形状を設定
//	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//}
//
//void Object3Ds::PostDraw()
//{
//	// コマンドリストを解除
//	Object3Ds::cmdList = nullptr;
//}

Object3Ds* Object3Ds::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	Object3Ds* object3d = new Object3Ds();
	if (object3d == nullptr) {
		return nullptr;
	}

	//すけーるをセット
	float scale_val = 20;
	object3d->scale = { scale_val,scale_val ,scale_val };

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}

//PipelineSet Object3Ds::CreatePipeline()
//{
//	LoadHlsl(L"BasicVS.hlsl", &_vsBlob, "main", "vs_5_0");
//	LoadHlsl(L"BasicPS.hlsl", &_psBlob, "main", "ps_5_0");
//
//	// 頂点レイアウト
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//		//頂点座標
//		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//法線
//		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//uv
//		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//ボーン番号
//		{"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//ウェイト
//		{"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//		//輪郭線フラグ
//		{"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
//	};
//
//	gpipeline.pRootSignature = nullptr;//後で設定
//
//	//シェーダー
//	gpipeline.VS = CD3DX12_SHADER_BYTECODE(_vsBlob);
//	//gpipeline.GS = CD3DX12_SHADER_BYTECODE(_gsBlob.Get());
//	gpipeline.PS = CD3DX12_SHADER_BYTECODE(_psBlob);
//
//	//サンプルマスク,ラスタライザー
//	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//背面カリングあり
//	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//背面カリングなし
//
//	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//デフォルトのサンプルマスクを表す定数
//
//	//レンダターゲットの設定
//	gpipeline.NumRenderTargets = 1;//今は1つ
//	gpipeline.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;//0～1に正規化されたRGBA
//
//	//深度ステンシル
//	gpipeline.DepthStencilState.DepthEnable = true;//使う
//	gpipeline.DepthStencilState.StencilEnable = false;//
//	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込む
//	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さい方を採用
//	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
//
//	//ブレンドステートの設定
//	gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//
//	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlenddesc = {};
//	renderTargetBlenddesc.BlendEnable = false;
//	renderTargetBlenddesc.LogicOpEnable = false;
//	renderTargetBlenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//	gpipeline.BlendState.RenderTarget[0] = renderTargetBlenddesc;
//
//	//入力レイアウトの設定
//	gpipeline.InputLayout.pInputElementDescs = inputLayout;//レイアウトの先頭アドレス
//	gpipeline.InputLayout.NumElements = _countof(inputLayout);//レイアウト配列の要素
//	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//カットなし
//	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成
//
//	//アンチエイリアシングのためのサンプル数設定
//	gpipeline.SampleDesc.Count = 1;
//	gpipeline.SampleDesc.Quality = 0;
//
//	PipelineSet pipelineSet;
//	//デスクリプタレンジ
//	//定数用
//	CD3DX12_DESCRIPTOR_RANGE descRangeCBV = {};
//	descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
//
//	//テクスチャ用
//	CD3DX12_DESCRIPTOR_RANGE descRangeSRV = {};
//	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
//
//	//サンプラー
//	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};
//	samplerDesc.Init(0);
//
//	//ルートパラメーター
//	CD3DX12_ROOT_PARAMETER rootparam[2] = {};
//	rootparam[0].InitAsConstantBufferView(0);
//	rootparam[1].InitAsDescriptorTable(1, &descRangeSRV);
//
//	//ルートシグネチャ
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//	rootSignatureDesc.pParameters = rootparam;
//	rootSignatureDesc.NumParameters = 2;
//	rootSignatureDesc.pStaticSamplers = &samplerDesc;
//	rootSignatureDesc.NumStaticSamplers = 1;
//
//	//バージョン自動生成でのシリアライズ
//	auto result = D3D12SerializeRootSignature(
//		&rootSignatureDesc,
//		D3D_ROOT_SIGNATURE_VERSION_1_0,
//		&_rootSigBlob,
//		_errorBlob.ReleaseAndGetAddressOf());
//
//	//ルートシグネチャの生成
//	result = device->CreateRootSignature(
//		0,
//		_rootSigBlob->GetBufferPointer(),
//		_rootSigBlob->GetBufferSize(),
//		IID_PPV_ARGS(pipelineSet._rootsignature.ReleaseAndGetAddressOf()));
//
//	//パイプラインにルートシグネチャをセット
//	gpipeline.pRootSignature = pipelineSet._rootsignature.Get();
//
//	//グラフィックスパイプラインステートオブジェクトの生成
//	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineSet._pipelinestate.ReleaseAndGetAddressOf()));
//
//	return pipelineSet;
//}

//void Object3Ds::CreateObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap)
//{
//	for (int i = 0; i < index; i++) {
//		//大きさ、回転角、座標を指定
//		//object3ds[i].parent = &object3ds[i - 1];
//		object[i].scale = { 1.0f,1.0f,1.0f };
//		object[i].rotation = { 0.0f,0.0f,0.0f };
//		object[i].position = { 0.0f,5.0f,100.0f };
//		object[i].Flag = false;
//	}
//}

void Object3Ds::SetModel(Model* model)
{
	//assert(!model);

	this->model = model;
	model->Initialize();
}

//void Object3Ds::SetPmdModel(PMDmodel* pmdModel)
//{
//	this->pmdModel = pmdModel;
//	pmdModel->Initialize();
//
//}
//
//void Object3Ds::InitalizeCamera(int window_width, int window_height)
//{
//	// ビュー行列の生成
//	matView = XMMatrixLookAtLH(
//		XMLoadFloat3(&eye),
//		XMLoadFloat3(&target),
//		XMLoadFloat3(&up));
//
//	// 平行投影による射影行列の生成
//	//constMap->mat = XMMatrixOrthographicOffCenterLH(
//	//	0, window_width,
//	//	window_height, 0,
//	//	0, 1);
//	// 透視投影による射影行列の生成
//	matProjection = XMMatrixPerspectiveFovLH(
//		XMConvertToRadians(60.0f),
//		(float)window_width / window_height,
//		0.1f, 1000.0f
//	);
//}

bool Object3Ds::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/OBJVertexShader.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/OBJPixelShader.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[3];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&_rootsignature));
	if (FAILED(result)) {
		return result;
	}

	gpipeline.pRootSignature = _rootsignature.Get();

	// グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));

	if (FAILED(result)) {
		return result;
	}

	return true;
}

bool Object3Ds::Initialize()
{
	// nullptrチェック
	assert(device);

	HRESULT result;
	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	return true;
}

void Object3Ds::Update()
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// 定数バッファへデータ転送(OBJ)
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	//constMap->mat = matWorld * matView * matProjection;	// 行列の合成
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constBuffB0->Unmap(0, nullptr);

	////// 定数バッファへデータ転送(PMD)
	////ConstBufferDataB0* pConstMap = nullptr;
	////result = PMDconstBuffB0->Map(0, nullptr, (void**)&pConstMap);
	////pConstMap->viewproj = matProjection;
	////pConstMap->world = matWorld;
	////pConstMap->cameraPos = cameraPos;
	////PMDconstBuffB0->Unmap(0, nullptr);
	////model->Update();
}

void Object3Ds::Draw()
{
	// nullptrチェック
	assert(device);
	assert(Object3Ds::cmdList);

	// オブジェクトモデルの割り当てがなければ描画しない
	if (model == nullptr) {
		return;
	}

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &model->VbView());
	// インデックスバッファの設定
	cmdList->IASetIndexBuffer(&model->IbView());

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { model->DescHeap() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// パイプラインステートの設定
	cmdList->SetPipelineState(_pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(_rootsignature.Get());

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// モデル描画
	model->Draw(Object3Ds::cmdList);

	cmdList->DrawIndexedInstanced((UINT)model->Indices().size(), 1, 0, 0, 0);
}
