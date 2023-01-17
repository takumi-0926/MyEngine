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

bool Weapon::Initialize()
{
	return Object3Ds::Initialize();
}

void Weapon::Update()
{
	Object3Ds::Update();

	// 定数バッファへデータ転送(OBJ)
	ConstBufferDataB0* constMap = nullptr;
	auto result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	if (FAILED(result)) { assert(0); }
	constMap->world = matWorld * FollowingObjectBoneMatrix;
	constBuffB0->Unmap(0, nullptr);
}

void Weapon::Draw()
{
	Object3Ds::Draw();
}
