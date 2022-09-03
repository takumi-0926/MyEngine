#pragma once
#include"..\object\object3D.h"
#include "..\bullet.h"
#include "..\enemy.h"

class Enemy;
class Bullet;
class DefCannon : public Object3Ds{
	static Bullet* bullet[10];
	struct BulletStatus {
		float speed;
		XMVECTOR vec;
		bool isAlive;
	};
public:
	DefCannon();

	static DefCannon* Create(Model* model);

	float	 distance(XMFLOAT3 pos1, XMFLOAT3 pos2);	//目標との直線距離を計算
	XMVECTOR objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2);//目標までのベクトルを算出
	XMVECTOR normalize(XMVECTOR vec);					//正規化
	void moveUpdate(std::vector<Enemy*> ePos);
	XMFLOAT3 GetPosition() { return position; }

	void Update()override;
	void Draw()override;
};