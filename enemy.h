#pragma once
#include "..\object\object3D.h"
#include "..\PMD\PMDmodel.h"
#include "..\stageObject.h"
#include "..\DefCannon.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"

class PMDmodel;
class StageObject;
class DefCannon;
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
	bool alive = false;	//��������
	bool Move = true;  //�ړ��t���O
	bool attack = false;//�U���t���O
	bool startAttack = false;
	bool attackHit = true;
	bool attackOnMove = false;
	bool damage = false;

	float step = 0.00005f;	//�i�s
	float pct = 0.0f;		//�o��
	float attackTime = 0.0f;
	int mode = 0;

	float damegeCount = 0.0f;

	XMVECTOR vectol;
	XMFLOAT3 attackPos;
	XMFLOAT3 oldPos;
private:
	XMFLOAT3 VectorToXMFloat(XMVECTOR vec);
	float	 objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2);	//�����Ƃ̒����������v�Z
	XMVECTOR objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2);	//�����Ԃ̃x�N�g�����v�Z
	XMVECTOR Normalize(XMVECTOR vec);
	void move(XMVECTOR vector);	//�x�N�g���ړ�
	void moveReset();
public:
	Enemy();		//�R���X�g���N�^
	static Enemy* Create();//�C���X�^���X����
	void Update() override;
	void moveUpdate(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos);
	void Draw() override;

	/// <summary>
	/// �i�s�����ɉ�]
	/// </summary>
	/// <param name="forward">�i�s�����x�N�g��</param>
	/// <param name="upward">��x�N�g��</param>
	/// <returns>��]�s��i�N�H�[�^�j�I���j</returns>
	XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward) {
		Vector3 z = Vector3(forward.x, forward.y, forward.z);//�i�s�����x�N�g���i�O�����j
		Vector3 up = Vector3(upward.x, upward.y, upward.z);  //�����
		XMMATRIX rot;//��]�s��
		Quaternion q = quaternion(0, 0, 0, 1);//��]�N�H�[�^�j�I��
		Vector3 _z = { 0.0f,0.0f,-1.0f };//Z�����P�ʃx�N�g��
		Vector3 cross;

		float a;//�p�x�ۑ��p
		float b;//�p�x�ۑ��p
		float c;//�p�x�ۑ��p
		float d;//�p�x�ۑ��p

		cross = z.cross(_z);

		q.x = cross.x;
		q.y = cross.y;
		q.z = cross.z;

		q.w = sqrt(
			(z.length() * z.length())
			* (_z.length() * _z.length())) + z.dot(_z);

		//�P�ʃN�H�[�^�j�I����
		
		q = normalize(q);
		q = conjugate(q);
		a = q.x;
		b = q.y;
		c = q.z;
		d = q.w;

		//�C�ӎ���]
		XMVECTOR rq = { q.x,q.y,q.z,q.w };
		rot = XMMatrixRotationQuaternion(rq);

		this->useRotMat = true;
		return rot;
	}
};