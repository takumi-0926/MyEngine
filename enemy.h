#pragma once
#include "..\object\object3D.h"
#include "..\PMD\PMDmodel.h"
#include "..\stageObject.h"

class PMDmodel;
class StageObject;
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
	bool alive;	//生存判定

	float step = 0.00005f;	//進行
	float pct = 0.0f;		//経過

	int mode = 0;
private:
	float objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2);				//建物との直線距離を計算
	XMFLOAT3 moveObject(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct);	//指定の建物へ移動

public:
	Enemy();		//コンストラクタ
	static Enemy* Create();//インスタンス生成
	void Update() override;
	void moveUpdate(XMFLOAT3 pPos, StageObject *bPos[], XMFLOAT3 gPos);
	void Draw() override;

};