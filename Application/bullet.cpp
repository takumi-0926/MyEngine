#include "bullet.h"
#include "Collision/CollisionManager.h"
#include "Collision/SphereCollider.h"
#include "Collision/CollisionAttribute.h"
#include "Collision/QueryCallBack.h"

#include <Math/Vector3.h>
#include <Math/Quaternion.h>

Bullet::Bullet()
{
}
Bullet* Bullet::Create(Model* _model)
{
	// 3Dオブジェクトのインスタンスを生成
	Bullet* instance = new Bullet();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	if (_model) {
		instance->SetModel(_model);
	}

	return instance;
}
bool Bullet::Initialize()
{
	Object3Ds::Initialize();

	return true;
}
void Bullet::Update()
{
	UpdateWorldMatrix();

	Object3Ds::Update();

	if (this->status.isAlive == true) {
		this->position.x += -this->status.vec.m128_f32[0] * this->status.speed;
		this->position.y += -this->status.vec.m128_f32[1] * this->status.speed;
		this->position.z += -this->status.vec.m128_f32[2] * this->status.speed;

		SetMatRot(LookAtRotation(status.vec, XMFLOAT3(0, 1, 0)));

		collider->Update();
	}
	else {
		this->position = this->status.basePos;
		CollisionManager::GetInstance()->RemoveCollider(collider);
		return;
	}
	count += 1.0f / 60.0f;
	if (count >= 3.0f) {
		status.isAlive = false;
		count = 0;
	}
}
void Bullet::Draw()
{
	Object3Ds::Draw();
}

void Bullet::OnCollision(const CollisionInfo& info)
{
	status.isAlive = false;
}

XMMATRIX Bullet::LookAtRotation(XMVECTOR forward, XMFLOAT3 upward)
{
	Vector3 z = Vector3(forward.m128_f32[0], forward.m128_f32[1], forward.m128_f32[2]);//進行方向ベクトル（前方向）
	Vector3 up = Vector3(upward.x, upward.y, upward.z);  //上方向
	XMMATRIX rot;//回転行列
	Quaternion q = quaternion(0, 0, 0, 1);//回転クォータニオン
	Vector3 _z = { 0.0f,0.0f,-1.0f };//Z方向単位ベクトル
	Vector3 cross;

	float a;//角度保存用
	float b;//角度保存用
	float c;//角度保存用
	float d;//角度保存用

	cross = z.cross(_z);

	q.x = cross.x;
	q.y = cross.y;
	q.z = cross.z;

	q.w = sqrt(
		(z.length() * z.length())
		* (_z.length() * _z.length())) + z.dot(_z);

	//単位クォータニオン化

	q = normalize(q);
	q = conjugate(q);
	a = q.x;
	b = q.y;
	c = q.z;
	d = q.w;

	//任意軸回転
	XMVECTOR rq = { q.x,q.y,q.z,q.w };
	rot = XMMatrixRotationQuaternion(rq);

	this->useRotMat = true;
	return rot;
}

void Bullet::SetAlive(bool flag){
	if (flag == true) {
		float radius = 2.0f;
		SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
		collider->SetAttribute(COLLISION_ATTR_BULLET);
	}
	this->status.isAlive = flag;
}
