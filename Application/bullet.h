#pragma once
#include "object\object3D.h"

class Bullet : public Object3Ds {
	struct Status {
		XMVECTOR vec;
		bool vecSet;
		XMFLOAT3 basePos;
		float speed;
		bool isAlive;
	};
	Status status;

public:
	Bullet();
	static Bullet* Create(Model* _model);//インスタンス生成
	bool Initialize();
	void Update() override;
	void Draw() override;

	void OnCollision(const CollisionInfo& info) override;
	XMMATRIX LookAtRotation(XMVECTOR forward, XMFLOAT3 upward);

	void SetAlive(bool flag);
	void SetVec(XMVECTOR vec) { this->status.vec = vec; }
	void SetVecSet(bool flag) { this->status.vecSet = flag; }
	void SetSpeed(float speed) { this->status.speed = speed; }
	void SetBasePos(XMFLOAT3 pos) { this->status.basePos = pos; }

	Status GetStatus() { return status; }
public:

	float attackCount = 1.0f;
	float count = 0.0f;
};