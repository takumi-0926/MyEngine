#include "object3D.h"
#include "..\Application\dx12Wrapper.h"
#include "..\pipelineSet.h"

// デバイス
ID3D12Device* Object3Ds::device;
// コマンドリスト
ID3D12GraphicsCommandList* Object3Ds::cmdList;

Wrapper* Object3Ds::dx12 = nullptr;

bool Object3Ds::StaticInitialize(ID3D12Device* _device, SIZE _ret)
{
	// 再初期化チェック
	assert(!Object3Ds::device);

	// nullptrチェック
	assert(_device);

	Object3Ds::device = _device;

	//パイプライン生成
	LoadHlsls::LoadHlsl_VS(ShaderNo::OBJ, L"Resources/shaders/OBJVertexShader.hlsl", "main", "vs_5_0");
	LoadHlsls::LoadHlsl_PS(ShaderNo::OBJ, L"Resources/shaders/OBJPixelShader.hlsl", "main", "ps_5_0");
	LoadHlsls::createPipeline(device, ShaderNo::OBJ);

	Model::StaticInitialize(_device);

	return true;
}

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

void Object3Ds::SetModel(Model* model)
{
	this->model = model;
}

bool Object3Ds::Initialize()
{
	// nullptrチェック
	assert(device);

	this->dx12 = dx12;

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

	const XMMATRIX& matViewProjection = dx12->Camera()->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = dx12->Camera()->GetEye();

	// 定数バッファへデータ転送(OBJ)
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	if (FAILED(result)) {
		assert(0);
	}

	//constMap->mat = matWorld * matView * matProjection;	// 行列の合成
	constMap->viewproj = matViewProjection;
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constBuffB0->Unmap(0, nullptr);
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

	//// パイプラインステートの設定
	//cmdList->SetPipelineState(_pipelinestate.Get());
	//// ルートシグネチャの設定
	//cmdList->SetGraphicsRootSignature(_rootsignature.Get());
	// パイプラインステートの設定
	cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNo::OBJ)._pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNo::OBJ)._rootsignature.Get());

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	dx12->DrawLight(cmdList);

	// モデル描画
	model->Draw(cmdList);
}
