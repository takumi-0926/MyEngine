#include "object3D.h"
#include "dx12Wrapper.h"
#include "pipelineSet.h"
#include "Collision\BaseCollision.h"
#include "Collision\CollisionManager.h"

Wrapper* Object3Ds::dx12 = nullptr;
Camera* Object3Ds::camera = nullptr;

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
	LoadHlsls::LoadHlsl_VS(ShaderNo::OBJ, L"Resources/shaders/OBJVertexShader.hlsl", "main", "vs_5_0");
	LoadHlsls::LoadHlsl_PS(ShaderNo::OBJ, L"Resources/shaders/OBJPixelShader.hlsl", "main", "ps_5_0");
	LoadHlsls::createPipeline(device.Get(), ShaderNo::OBJ);

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
		const XMMATRIX& matViewProjection = dx12->Camera()->GetViewProjectionMatrix();
		const XMFLOAT3& cameraPos = dx12->Camera()->GetEye();

		UpdateWorldMatrix();

		// 定数バッファへデータ転送(OBJ)
		ConstBufferDataB0* constMap = nullptr;
		result = constBuffB0->Map(0, nullptr, (void**)&constMap);
		if (FAILED(result)) { assert(0); }
		constMap->viewproj = matViewProjection;
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
	cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNo::OBJ)._pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNo::OBJ)._rootsignature.Get());

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

void Object3Ds::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	CollisionManager::GetInstance()->AddCollider(collider);
	UpdateWorldMatrix();
	collider->Update();
}
