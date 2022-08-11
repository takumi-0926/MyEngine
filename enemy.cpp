#include "enemy.h"

float Enemy::objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	float distance;
	float x = abs(pos1.x - pos2.x);
	float z = abs(pos1.z - pos2.z);
	distance = std::sqrt(x * 2 + z * 2);
	return distance;
}

XMFLOAT3 Enemy::moveObject(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct)
{
	XMFLOAT3 pos;
	pos.x = pos1.x + ((pos2.x - pos1.x) * pct);
	pos.z = pos1.z + ((pos2.z - pos1.z) * pct);
	pos.y = pos1.y;
	return pos;
}

Enemy::Enemy()
{
	alive = false;
	step = 0.00005f;
	pct = 0.0f;
	mode = 0;
}

Enemy* Enemy::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	Enemy* object3d = new Enemy();
	if (object3d == nullptr) {
		return nullptr;
	}

	object3d->position.y = 20;
	object3d->position.z = -150;

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}

void Enemy::Update() {
	Object3Ds::Update();
}

void Enemy::moveUpdate(XMFLOAT3 pPos, Object3Ds* bPos[],XMFLOAT3 gPos)
{
	if (mode == 1) {

		int objectNo = 0;
		float distance = 1000;
		for (int i = 0; i < 2; i++)
		{
			float dis = objectDistance(position, bPos[i]->position);
			if (distance >= dis) {
				distance = dis;
				objectNo = i;
			}
		}

		if (pct <= 1.0f) {
			position = moveObject(position, bPos[objectNo]->position, pct);
			pct += this->step;
		}
	}
	if (mode == 2) {
		if (pct <= 1.0f) {
			position = moveObject(position, pPos, pct);
			pct += this->step;
		}
	}
	if (mode == 3) {
		if (pct <= 1.0f) {
			position = moveObject(position, gPos, pct);
			pct += step;
		}
	}

	Object3Ds::Update();
	//Update();
}

void Enemy::Draw()
{
	Object3Ds::Draw();
}
