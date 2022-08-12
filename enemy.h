#pragma once
#include "..\object\object3D.h"
#include "..\PMD\PMDmodel.h"
#include "..\stageObject.h"

class PMDmodel;
class StageObject;
class Enemy : public Object3Ds {
	struct Status {
		int HP;
		float attack;	//�U����
		float defense;	//�h���
		float speed;
		XMFLOAT3 scale;	//�傫��
	};

public:
	Status status;
	bool alive;	//��������

	float step = 0.00005f;	//�i�s
	float pct = 0.0f;		//�o��

	int mode = 0;
private:
	float objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2);				//�����Ƃ̒����������v�Z
	XMFLOAT3 moveObject(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct);	//�w��̌����ֈړ�

public:
	Enemy();		//�R���X�g���N�^
	static Enemy* Create();//�C���X�^���X����
	void Update() override;
	void moveUpdate(XMFLOAT3 pPos, StageObject *bPos[], XMFLOAT3 gPos);
	void Draw() override;

};