#pragma once
#include "..\object\object3D.h"
#include "..\PMD\PMDmodel.h"
#include "..\stageObject.h"
#include "..\DefCannon.h"

class PMDmodel;
class StageObject;
class DefCannon;
class Enemy : public Object3Ds {
	struct Status {
		int HP;
		float attack;	//攻撃力
		float defense;	//防御力
		float speed;
		XMFLOAT3 scale;	//大きさ
	};

public:
	Status status;
	bool alive = false;	//生存判定
	bool move = true;  //移動フラグ
	bool attack = false;//攻撃フラグ
	bool startAttack = false;
	bool attackHit = true;
	bool attackOnMove = false;
	bool damage = false;

	float step = 0.00005f;	//進行
	float pct = 0.0f;		//経過
	float attackTime = 0.0f;
	int mode = 0;

	float damegeCount = 0.0f;

	XMVECTOR vectol;
	XMFLOAT3 attackPos;
	XMFLOAT3 oldPos;
private:
	float	 objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2);				//建物との直線距離を計算
	XMVECTOR objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2);				//建物間のベクトルを計算
	XMVECTOR normalize(XMVECTOR vec);
	XMFLOAT3 moveObject(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct);	//指定の建物へ移動
	void moveReset();
public:
	Enemy();		//コンストラクタ
	static Enemy* Create();//インスタンス生成
	void Update() override;
	void moveUpdate(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos);
	void Draw() override;

};