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
	float	 distance(XMFLOAT3 pos1, XMFLOAT3 pos2);	//�ڕW�Ƃ̒����������v�Z
	XMVECTOR objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2);//�ڕW�܂ł̃x�N�g�����Z�o
	XMVECTOR Normalize(XMVECTOR vec);					//���K��
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
	/// �i�s�����ɉ�]
	/// </summary>
	/// <param name="forward">�i�s�����x�N�g��</param>
	/// <param name="upward">��x�N�g��</param>
	/// <returns>��]�s��i�N�H�[�^�j�I���j</returns>
	XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward);

	inline void SetMode(int mode) { this->mode = mode; }
	inline void SetShotVec(const XMVECTOR& vec) { this->shotVec = vec; }

	inline bool GetAlive() { return isAlive; }
	inline void SetAlive(bool flg) { this->isAlive = flg; }

public:
};