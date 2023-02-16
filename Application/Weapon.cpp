#include "Weapon.h"

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

	instance->scale = XMFLOAT3(0.001f, 0.001f, 0.001f);

	// 初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	//モデルデータセット
	if (model) {
		instance->SetModel(model);
	}

	return instance;
}

//bool Weapon::Initialize()
//{
//	Object3Ds::Initialize();
//	return true;
//}

void Weapon::Update()
{
	//matWorld = matWorld * FollowingObjectBoneMatrix;

	//Object3Ds::Update();

	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();
	const XMFLOAT3& cameraPos = camera->GetEye();

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
	matWorld *= FollowingObjectBoneMatrix;

	//position.x = matWorld.r[0].m128_f32[3];
	//position.y = matWorld.r[1].m128_f32[3];
	//position.z = matWorld.r[2].m128_f32[3];

	ConstBufferDataB0* constMap = nullptr;
 	auto result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	if (FAILED(result)) { assert(0); }
	constMap->viewproj = matViewProjection;
	constMap->cameraPos = cameraPos;
	constMap->world = matWorld;
	constBuffB0->Unmap(0, nullptr);
}

void Weapon::Draw()
{
	Object3Ds::Draw();
}
