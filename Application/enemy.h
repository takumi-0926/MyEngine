#pragma once
#include "object\object3D.h"
#include "PMD\PMDmodel.h"
#include "DefCannon.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

#include "FBX/FbxObject3d.h"

#include "Weapon.h"
#include "ParticleManager.h"

enum Activity {
	wolf = 0,
	golem,
};

class PMDmodel;
class StageObject;
class DefCannon;
class Enemy : public FbxObject3d {
	struct Status {//28
		int HP;
		float attack;	//�U����
		float defense;	//�h���
		float speed;
		XMFLOAT3 scale;	//�傫��
	};

public:
	Status status;//28
	bool alive = false;	//��������
	bool startAttack = false;
	bool attackHit = true;
	bool attackOnMove = false;
	bool damage = false;

	int actionPattern = 0;//�s���p�^�[��
	float attackTime = 0.0f;
	int mode = 0;

	float damegeCount = 0.0f;

	unsigned short myNumber = 0;
	int registrationNumber = 0;

	float particleOffset = 0.0f;
	//�n�ʔ���p
	bool OnGround = true;
	XMVECTOR fallV;

	float shadowOffset = 0.0f;

	int followBoneNum = 0;

	float alpha = 1.0f;
	vector<XMFLOAT3> defalt_ambient;
	XMVECTOR vectol;
	XMFLOAT3 attackPos;
	XMFLOAT3 oldPos;
	XMFLOAT3 RetreatPos = { 1.0f,20.0f,-150.0f };

	FbxModel* modelType[2] = {};
	Weapon* weapon = nullptr;
	ParticleManager* particle = nullptr;

private:
	XMFLOAT3 VectorToXMFloat(XMVECTOR vec);
	float	 objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2);	//�����Ƃ̒����������v�Z
	XMVECTOR objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2);	//�����Ԃ̃x�N�g�����v�Z
	XMVECTOR Normalize(XMVECTOR vec);
	void move(XMVECTOR vector);	//�x�N�g���ړ�
	void moveReset();
public:
	Enemy();		//�R���X�g���N�^
	static Enemy* Create(FbxModel* model1, FbxModel* model2);//�C���X�^���X����
	void Initialize()override;
	void Update() override;
	void moveUpdate(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos);
	void Draw(ID3D12GraphicsCommandList* cmdList) override;

	void OnCollision(const CollisionInfo& info)override;

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

	void SetAlpha() {
		//for (int i = 0; i < model->GetMesh().size(); i++) {
		//	this->model->GetMesh()[i]->GetMaterial()->alpha = this->alpha;
		//	this->model->GetMesh()[i]->GetMaterial()->Update();
		//}
		model->alpha = this->alpha;
		model->Update();
	}
	inline void SetFollowBoneNum(int num) { this->followBoneNum = num; }

	void CreateWeapon(Model* model);
	void Particle();

	/// <summary>
	/// �o������
	/// </summary>
	void Appearance();

	/// <summary>
	/// �ړ�������
	/// </summary>
	void Move(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos);

	/// <summary>
	/// �U��������
	/// </summary>
	void Attack(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos);

	/// <summary>
	/// �ދp������
	/// </summary>
	void Retreat();

	/// <summary>
	/// ��_���[�W������
	/// </summary>
	void Damage();

	void ChangeAnimation(int num);
};