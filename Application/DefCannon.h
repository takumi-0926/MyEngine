#pragma once
#include"object\object3D.h"
#include "bullet.h"
#include "enemy.h"

class Enemy;
class Bullet;
class DefCannon : public Object3Ds{

	std::vector<Bullet*> bullet;
	float count = 0.0f;

	float	 distance(XMFLOAT3 pos1, XMFLOAT3 pos2);	//�ڕW�Ƃ̒����������v�Z
	XMVECTOR objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2);//�ڕW�܂ł̃x�N�g�����Z�o
	XMVECTOR normalize(XMVECTOR vec);					//���K��

public:
	DefCannon();
	static DefCannon* Create();
	void BulletCreate(Model* _model);
	void Update()override;
	void Draw()override;

	void moveUpdate(std::vector<Enemy*> ePos);

public:
	bool isAlive = true;
	int HP = 2;
};