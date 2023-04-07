#include "object3D.h"
#include "dx12Wrapper.h"
#include "pipelineSet.h"
#include "Collision\BaseCollision.h"
#include "Collision\CollisionManager.h"

Wrapper* Object3Ds::dx12 = nullptr;
Camera* Object3Ds::camera = nullptr;

ComPtr<ID3D12RootSignature> Object3Ds::_rootsignature;
ComPtr<ID3D12PipelineState> Object3Ds::_pipelinestate;
ComPtr<ID3D12PipelineState> Object3Ds::_plsShadow;

//ImGui確認用
float Object3Ds::shadowCameraSite[2] = {160.0f,160.0f};

Object3Ds::~Object3Ds()
{
	if (collider) {
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
}

bool Object3Ds::StaticInitialize(ID3D12Device* _device)
{
	// 再初期化チェック
	assert(!Object3Ds::device);

	// nullptrチェック
	assert(_device);

	Object3Ds::device = _device;

	//パイプライン生成
	//LoadHlsls::LoadHlsl_VS(ShaderNo::OBJ, L"Resources/shaders/OBJVertexShader.hlsl", "main", "vs_5_0");
	//LoadHlsls::LoadHlsl_PS(ShaderNo::OBJ, L"Resources/shaders/OBJPixelShader.hlsl", "main", "ps_5_0");
	//LoadHlsls::createPipeline(device.Get(), ShaderNo::OBJ);

	HRESULT result;
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;

	//シェーダー読み込み
	{

		//指定(namePath)のhlslファイルをロード
		auto result = D3DCompileFromFile(
			L"Resources/shaders/OBJVertexShader.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&vsBlob, &errorBlob);
		//読み込み成功チェック
		if (FAILED(result)) {
			if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
				::OutputDebugStringA("ファイルが見当たりません");
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

		//指定(namePath)のhlslファイルをロード
		result = D3DCompileFromFile(
			L"Resources/shaders/OBJPixelShader.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&psBlob, &errorBlob);
		//読み込み成功チェック
		if (FAILED(result)) {
			if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
				::OutputDebugStringA("ファイルが見当たりません");
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
	}

	ComPtr<ID3DBlob> rootSigBlob;
	PipelineSet pipelineset;

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
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};
	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	CD3DX12_ROOT_SIGNATURE_DESC _rootSignatureDesc = {};

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

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

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV[2]{};
	descRangeSRV[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
	descRangeSRV[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[5]{};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV[0], D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[4].InitAsDescriptorTable(1, &descRangeSRV[1]);//

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
	samplerDescs[0].Init(0);
	samplerDescs[1].Init(1, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);


	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	//ComPtr<ID3DBlob> rootSigBlob = nullptr;
	//ComPtr<ID3DBlob> errorBlob = nullptr;
	// ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(_rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = _rootsignature.Get();

	// グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(_pipelinestate.ReleaseAndGetAddressOf()));

	result = D3DCompileFromFile(//VS
		L"Resources/shaders/shadowShader.hlsl",//シェーダー名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"shadowVS", "vs_5_0",//関数、対象シェーダー
		0,
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

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS.BytecodeLength = 0;
	gpipeline.PS.pShaderBytecode = nullptr;
	gpipeline.NumRenderTargets = 0;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

	result = device->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(_plsShadow.ReleaseAndGetAddressOf())
	);

	Model::StaticInitialize(_device);

	return true;
}

Object3Ds* Object3Ds::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	Object3Ds* object3d = new Object3Ds();
	if (object3d == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	if (model) {
		object3d->SetModel(model);
	}

	return object3d;
}

void Object3Ds::SetModel(Model* model)
{
	this->model = model;
}

bool Object3Ds::Initialize()
{
	// nullptrチェック
	assert(device);

	this->dx12 = dx12;

	name = typeid(*this).name();

	HRESULT result;
	// 定数バッファの生成
	CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);
	result = device->CreateCommittedResource(
		&properties, 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	return true;
}

void Object3Ds::Update()
{
	HRESULT result;
	//const XMMATRIX& matView = dx12->Camera()->GetViewMatrix();
	//const XMMATRIX& matProjection = dx12->Camera()->GetProjectionMatrix();

	if (!useWorldMat) {
		const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
		const XMFLOAT3& cameraPos = camera->GetEye();

		UpdateWorldMatrix();

		auto light = XMFLOAT4(1, -1, 0, 0);
		XMVECTOR lightVec = XMLoadFloat4(&light);

		auto Eye = XMFLOAT3(25, 30, 0);
		XMVECTOR eye = XMLoadFloat3(&Eye);
		auto Target = XMFLOAT3(0, 0, 0);
		XMVECTOR terget = XMLoadFloat3(&Target);
		XMVECTOR up = XMLoadFloat3(&camera->GetUp());

		XMVECTOR lightPos = eye;

		// 定数バッファへデータ転送(OBJ)
		ConstBufferDataB0* constMap = nullptr;
		result = constBuffB0->Map(0, nullptr, (void**)&constMap);
		if (FAILED(result)) { assert(0); }
		constMap->viewproj = matViewProjection;
		constMap->lightCamera = XMMatrixLookAtLH(lightPos, terget, up) * XMMatrixOrthographicLH(shadowCameraSite[0], shadowCameraSite[1], 1.0f, 100.0f);
		constMap->cameraPos = cameraPos;
		if (!useWorldMat) {
			constMap->world = matWorld;
		}
		constBuffB0->Unmap(0, nullptr);
	}

	if (collider) {
		collider->Update();
	}
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

	// パイプラインステートの設定
	cmdList->SetPipelineState(_pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(_rootsignature.Get());

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	dx12->DrawLight(cmdList.Get());

	dx12->DrawDepth();

	// モデル描画
	model->Draw(cmdList.Get());
}

void Object3Ds::ShadowDraw()
{
	// nullptrチェック
	assert(device);
	assert(Object3Ds::cmdList);

	// オブジェクトモデルの割り当てがなければ描画しない
	if (model == nullptr) {
		return;
	}

	// パイプラインステートの設定
	cmdList->SetPipelineState(_plsShadow.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(_rootsignature.Get());

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	dx12->DrawLight(cmdList.Get());

	// モデル描画
	model->Draw(cmdList.Get());
}

void Object3Ds::UpdateWorldMatrix()
{
	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	if (!useRotMat) {
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
		matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
		matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	}
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity();// 変形をリセット
	matWorld *= matScale;// ワールド行列に	スケーリングを反映
	matWorld *= matRot;	 // ワールド行列に	回転        を反映
	matWorld *= matTrans;// ワールド行列に	平行移動    を反映
}

void Object3Ds::UpdateImgui()
{
	//深度テクスチャID取得
	auto HeapGPUHandle = dx12->GetDescHeap()->GetGPUDescriptorHandleForHeapStart();
	//HeapGPUHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	ImTextureID texID = ImTextureID(HeapGPUHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2);

	ImGui::Begin("ShadowCameraTest");
	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);
	ImGui::InputFloat2("cameraSite", shadowCameraSite);
	ImGui::Image(texID, ImVec2(shadowCameraSite[0], shadowCameraSite[1]));
	ImGui::End();
}

void Object3Ds::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	CollisionManager::GetInstance()->AddCollider(collider);
	UpdateWorldMatrix();
	collider->Update();
}

void Object3Ds::SetColliderInvisible(bool flag)
{
	collider->SetInvisible(flag);
}
