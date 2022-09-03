#pragma once
#include "..\object\object3D.h"

class Bullet : public Object3Ds{
	struct Status {
		XMVECTOR vec;
		XMFLOAT3 basePos;
		float speed;
		bool isAlive;
	};

public:
	Bullet();
	static Bullet* Create();//インスタンス生成
	void Update() override;
	void Draw() override;

public:
	Status status;
	float attackCount = 5.0f;
	float count = 0.0f;
};