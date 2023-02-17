#pragma once
#include"object\object3D.h"
#include "bullet.h"
#include "enemy.h"

class Enemy;
class Bullet;

enum DefenceType {
	Hand,
	Assault,
	Sniper
};

enum ShotMode {
	Auto,
	Operation,
	Fixed
};

class DefCannon : public Object3Ds {

	friend Bullet;

	std::vector<Bullet*> bullet;
	float count = 0.0f;
	bool isAlive = false;
	int mode = 0;

	XMVECTOR shotVec = {};
public:
	float	 distance(XMFLOAT3 pos1, XMFLOAT3 pos2);	//目標との直線距離を計算
	XMVECTOR objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2);//目標までのベクトルを算出
	XMVECTOR Normalize(XMVECTOR vec);					//正規化
	XMFLOAT3 VectorToXMFloat(XMVECTOR vec);

	DefCannon();
	static DefCannon* Create(int _mode, Model* _model);
	void BulletCreate(Model* _model);
	void Update()override;
	void Draw()override;

	void moveUpdate(std::vector<Enemy*> ePos);
	static DefCannon* Appearance(int type, Model* hand, Model* assault, Model* sniper);
	void AutoShot(std::vector<Enemy*> ePos);
	void OperationShot();
	void FixedShot();

	/// <summary>
	/// 進行方向に回転
	/// </summary>
	/// <param name="forward">進行方向ベクトル</param>
	/// <param name="upward">上ベクトル</param>
	/// <returns>回転行列（クォータニオン）</returns>
	XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward);

	inline void SetMode(int mode) { this->mode = mode; }
	inline void SetShotVec(const XMVECTOR& vec) { this->shotVec = vec; }

	inline bool GetAlive() { return isAlive; }
	inline void SetAlive(bool flg) { this->isAlive = flg; }

public:
};