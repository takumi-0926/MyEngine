#include "Weapon.h"

#include "Collision/MeshCollider.h"
#include <Collision/CollisionAttribute.h>

Weapon::Weapon()
{
}
Weapon::~Weapon()
{
}

Weapon* Weapon::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	Weapon* instance = new Weapon();
	if (instance == nullptr) {
		return nullptr;
	}

	//instance->scale = XMFLOAT3(0.1f, 0.1f, 0.1f);

	//モデルデータセット
	if (model) {
		instance->SetModel(model);
	}

	// 初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}


	return instance;
}

bool Weapon::Initialize()
{
	Object3Ds::Initialize();

	//コライダー追加
	MeshCollider* collider = new MeshCollider();
	collider->ConstructTriangles(model);
	SetCollider(collider);
	collider->SetAttribute(COLLISION_ATTR_ALLIES);
	collider->SetInvisible(true);
	return true;
}

void Weapon::Update()
{
	Object3Ds::Update();

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity();// 変形をリセット
	matWorld *= matScale;// ワールド行列に	スケーリングを反映
	matWorld *= matRot;	 // ワールド行列に	回転        を反映
	matWorld *= matTrans;// ワールド行列に	平行移動    を反映
	matWorld *= FollowingObjectBoneMatrix;

	ConstBufferDataB0* constMap = nullptr;
	auto result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	if (FAILED(result)) { assert(0); }
	constMap->viewproj = matViewProjection;
	constMap->cameraPos = cameraPos;
	constMap->world = matWorld;
	constBuffB0->Unmap(0, nullptr);

	collider->Update();
}

void Weapon::Draw()
{
	Object3Ds::Draw();
}

void Weapon::OnCollision(const CollisionInfo& info)
{
}
