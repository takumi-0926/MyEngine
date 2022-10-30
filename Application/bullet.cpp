#include "bullet.h"

Bullet::Bullet()
{
}
Bullet* Bullet::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	Bullet* object3d = new Bullet();
	if (object3d == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}
void Bullet::Update()
{
	Object3Ds::Update();

	if (this->status.isAlive == true) {
		this->position.x += this->status.vec.m128_f32[0] * this->status.speed;
		this->position.y += this->status.vec.m128_f32[1] * this->status.speed;
		this->position.z += this->status.vec.m128_f32[2] * this->status.speed;
	}
	else {
		this->position = this->status.basePos;
	}
}
void Bullet::Draw()
{
	Object3Ds::Draw();
}
