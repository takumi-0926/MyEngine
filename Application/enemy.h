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
	golem = 1,
	wolf,
};
enum AttackType_Wolf {
	Type01_Walk,
	Type02_JumpWait,
	Type02_JumpAttack,
	Type03_ChargeAttack,
};
enum MotionType {
	WalkMotion = 0,
	AttackMotion_01,
	AttackMotion_02,
	AttackMotion_03,
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

	struct Jump {
		Vector3 pos;
		Vector3 p1;
		Vector3 p2;
		Vector3 p3;
	}jump;
public:
	//�n�ʔ���p
	bool OnGround = true;
	XMVECTOR fallV;

	Status status;//28
	bool alive = false;	//��������
	bool startAttack = false;
	bool attackHit = false;
	bool attackOnMove = false;
	bool damage = false;

	int actionPattern = 0;//�s���p�^�[��
	int attackPattern = 0;//�U���p�^�[��
	float attackTime = 0.0f;
	float jumpTime = 0.0f;
	float maxTime = 1.0f;
	int mode = 0;


	float damegeCount = 0.0f;
	//�̎��ʔԍ�
	unsigned short myNumber = 0;
	int registrationNumber = 0;
	//�p�[�e�B�N���ʒu����
	float particleOffset = 0.0f;
	//�e�ʒu����
	float shadowOffset = 0.0f;
	//�����x
	float alpha = 1.0f;
	//�f�t�H���g�F
	vector<XMFLOAT3> defalt_ambient;
	XMVECTOR vectol;
	XMFLOAT3 attackPos;
	XMFLOAT3 oldPos;
	XMFLOAT3 RetreatPos = { 1.0f,20.0f,-150.0f };

	FbxModel* model = {};
	Weapon* weapon = nullptr;
	ParticleManager* particle = nullptr;

private:
	//Vector�^��XMFLAT3�^�ɃL���X�g
	XMFLOAT3 VectorToXMFloat(XMVECTOR vec);
	//��_�Ԃ̋������Z�o
	float objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2);	
	//��_�Ԃ̃x�N�g�����Z�o
	XMVECTOR objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2);	
	//���K��
	XMVECTOR Normalize(XMVECTOR vec);
	void Move(XMVECTOR vector);	//�x�N�g���ړ�
	void moveReset();
public:
	Enemy();		//�R���X�g���N�^
	~Enemy();
	static unique_ptr<Enemy> Create(FbxModel* model, const int Type);//�C���X�^���X����
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
	void JumpAttack(XMFLOAT3& targetPosition);
	void FingerAttack(XMFLOAT3& targetPosition);
	/// <summary>
	/// �ދp������
	/// </summary>
	void Retreat();

	/// <summary>
	/// ��_���[�W������
	/// </summary>
	void Damage();
};