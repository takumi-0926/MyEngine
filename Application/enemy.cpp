#include "enemy.h"
#include "Collision/CollisionManager.h"
#include "Collision/SphereCollider.h"
#include "Collision/CollisionAttribute.h"
#include "Collision/QueryCallBack.h"

enum MoveMode {
	move,
	attack,
	retreat
};

XMFLOAT3 Enemy::VectorToXMFloat(XMVECTOR vec)
{
	XMFLOAT3 ret;
	ret.x = vec.m128_f32[0];
	ret.y = vec.m128_f32[1];
	ret.z = vec.m128_f32[2];
	return ret;
}
float Enemy::objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	float distance;
	float x = abs(pos1.x - pos2.x);
	float z = abs(pos1.z - pos2.z);
	distance = std::sqrt(x * 2 + z * 2);
	return distance;
}
XMVECTOR Enemy::objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	XMVECTOR distance;
	float x = pos1.x - pos2.x;
	float z = pos1.z - pos2.z;
	distance = { x,0,z,0 };
	return distance;

}
XMVECTOR Enemy::Normalize(XMVECTOR vec)
{
	Vector3 ret;
	ret.x = vec.m128_f32[0];
	ret.y = vec.m128_f32[1];
	ret.z = vec.m128_f32[2];
	ret.normalize();

	XMVECTOR _ret;
	_ret.m128_f32[0] = ret.x;
	_ret.m128_f32[1] = ret.y;
	_ret.m128_f32[2] = ret.z;
	_ret.m128_f32[3] = 0;
	return _ret;
}
void Enemy::move(XMVECTOR vector)
{
	this->position.x -= vector.m128_f32[0] * this->status.speed;
	this->position.y -= vector.m128_f32[1] * this->status.speed;
	this->position.z -= vector.m128_f32[2] * this->status.speed;
}

Enemy::Enemy()
{
	status = {
					   2,//�f�t�H���gHP
					1.0f,//�f�t�H���g�U����
					1.0f,//�f�t�H���g�h���
					0.2f,//�f�t�H���g���x
		{1.0f,1.0f,1.0f},//�f�t�H���g�傫��
	};
	alive = false;
	mode = 0;
	vectol = {};
	oldPos = {};
	attackPos = {};
}
Enemy* Enemy::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Enemy* instance = new Enemy();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->position.y = 20;
	instance->position.z = -150;

	//�A���r�G���g�����擾
	for (int i = 0; i < model->GetMesh().size(); i++) {
		instance->defalt_ambient.push_back(model->GetMesh()[i]->GetMaterial()->ambient);
	}

	// ������
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	if (model) {

		Model* _model = model;
		instance->SetModel(_model);
	}

	return instance;
}
bool Enemy::Initialize()
{
	if (!Object3Ds::Initialize()) {
		return false;
	}

	float radius = 10.0f;

	SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));

	return true;
}
void Enemy::Update() {

	UpdateWorldMatrix();
	collider->Update();

	//���R���C�_�[�擾
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);

	/// <summary>
	/// ��p�N�G���[�R�[���o�b�N
	/// </summary>
	class EnemyQueryCallBack : public QueryCallBack {
	public:
		EnemyQueryCallBack(Sqhere* sphere) :sphere(sphere) {}

		bool OnQueryHit(const QueryHit& info) {
			const XMVECTOR up = { 0,1,0,0 };

			XMVECTOR rejectDir = XMVector3Normalize(info.reject);

			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			const float thrsehold = cosf(XMConvertToRadians(30.0f));
			//�p�x���ɂ���ĕǂ܂��͒n�ʂƔ��肳��Ă���ꍇ��������
			if (-thrsehold < cos && cos < thrsehold) {
				//���������o��
				sphere->center += info.reject;
				move += info.reject;
			}
			return true;
		}

		Sqhere* sphere = nullptr;
		XMVECTOR move = {};
	};

	EnemyQueryCallBack callback(sphereCollider);

	CollisionManager::GetInstance()->QuerySqhere(
		*sphereCollider,
		&callback,
		COLLISION_ATTR_LANDSHAPE
	);

	this->position.x += callback.move.m128_f32[0];
	this->position.y += callback.move.m128_f32[1];
	this->position.z += callback.move.m128_f32[2];

	UpdateWorldMatrix();

	//if (!alive) {
	//	Retreat();
	//}

	if (damage) {
		Damage();
	}

	collider->Update();

	if (!this->alive) {
		CollisionManager::GetInstance()->RemoveCollider(collider);
	}

	Object3Ds::Update();
}
void Enemy::Draw()
{
	SetAlpha();
	Object3Ds::Draw();
}

void Enemy::OnCollision(const CollisionInfo& info)
{
	info.collider;
}

Enemy* Enemy::Appearance(Model* model1, Model* model2)
{
	Enemy* ene = nullptr;
	static float popTime = 0;
	//�O�̂܂�
	if (popTime >= 10.0f) {
		int r = rand() % 10;
		if (r % 2 == 1) { ene = Enemy::Create(model2); }
		if (r % 2 != 1) { ene = Enemy::Create(model1); }
		if (r % 2 == 1) {
			ene->mode = 2;
			ene->position.y = 0;
			ene->scale = { 25,25,25 };
			ene->alive = true;
		}
		else if (r % 2 != 1) {
			ene->mode = 3;
			ene->position = { 0,0,-150 };
			ene->scale = { 3,3,3 };
			ene->alive = true;
		}

		popTime = 0;
	}
	else {
		popTime += 1.0f / 60.0f;
	}

	return ene;
}

void Enemy::Move(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{
	static int d = 7;
	int objectNo = 0;

	if (actionPattern != MoveMode::move)return;
	//�ړ�����
	//�p�^�[��1
	if (this->mode == 1) {
		float distance = 1000;//�����ۑ��p
		for (int i = 0; i < 6; i++)
		{
			//�����𑪒肵�čU���Ώۂ�����
			float dis = objectDistance(this->position, bPos[i]->position);
			if (distance >= dis) {
				if (bPos[i]->isAlive != true) { continue; }
				distance = dis;
				objectNo = i;
			}
		}

		move(Normalize(objectVector(this->position, bPos[objectNo]->position)));
		this->matRot = LookAtRotation(
			VectorToXMFloat(Normalize(objectVector(this->position, bPos[objectNo]->position))),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		//�U���Ώۂ��Ȃ��Ȃ����ꍇ
		for (int i = 0; i < 6; i++)
		{
			static int Num = 0;
			if (bPos[i]->isAlive == false) { Num++; }
			//�s���p�^�[����ω�
			if (Num == 6) {
				this->mode = 3;
			}
		}

		//�ړ�����U����
		if (objectDistance(this->position, bPos[objectNo]->position) <= d) {
			actionPattern = MoveMode::attack;
			this->attackOnMove = false;
		}
	}
	//�p�^�[��2
	if (this->mode == 2) {
		move(Normalize(objectVector(this->position, pPos)));
		this->matRot = LookAtRotation(
			VectorToXMFloat(Normalize(objectVector(this->position, pPos))),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		//�ړ�����U����
		if (objectDistance(this->position, pPos) <= d) {
			actionPattern = MoveMode::attack;
		}
	}
	//�p�^�[��3
	if (this->mode == 3) {
		move(Normalize(objectVector(this->position, gPos)));
		this->matRot = LookAtRotation(
			VectorToXMFloat(Normalize(objectVector(this->position, gPos))),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		//�ړ�����U����
		if (objectDistance(this->position, gPos) <= d) {
			actionPattern = MoveMode::attack;
			this->attackOnMove = false;
		}
	}
}

void Enemy::Attack(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{
	static int d = 7;
	int objectNo = 0;

	if (actionPattern != MoveMode::attack)return;

	//�U������
	if (this->mode == 1) {
		//�U�����̏��擾
		if (this->startAttack == false) {
			moveReset();
			this->vectol = objectVector(bPos[objectNo]->position, this->position);
			this->attackPos = this->position;
			this->startAttack = true;
		}

		if (this->attackTime >= 3.0f) {
			if (this->attackHit == true) {
				this->position.x += this->vectol.m128_f32[0] / 25;
				this->position.y += this->vectol.m128_f32[1] / 25;
				this->position.z += this->vectol.m128_f32[2] / 25;
				this->attackOnMove = true;
			}
			else {
				move(Normalize(objectVector(this->position, bPos[objectNo]->position)));
				this->matRot = LookAtRotation(
					VectorToXMFloat(Normalize(objectVector(this->position, gPos))),
					XMFLOAT3(0.0f, 1.0f, 0.0f));

			}
		}
		else if (this->attackTime >= 1.0f) {
			this->position.x -= this->vectol.m128_f32[0] / 100;
			this->position.y -= this->vectol.m128_f32[1] / 100;
			this->position.z -= this->vectol.m128_f32[2] / 100;
		}

		//�ړ������ڍs���̏�����
		if (this->position.x == this->attackPos.x && this->attackOnMove == true) {
			if (this->position.y == this->attackPos.y) {
				if (this->position.z == this->attackPos.z) {
					moveReset();
					this->attackTime = 0.0f;
					actionPattern = MoveMode::move;
					this->startAttack = false;
					this->attackHit = true;
				};
			};
		}

		this->attackTime += 1.0f / 60.0f;
	}
	if (this->mode == 2) {
		//�U�����̏��擾
		if (this->startAttack == false) {
			this->vectol = objectVector(pPos, this->position);
			this->attackPos = this->position;
			this->startAttack = true;
		}

		//�U���J�n�i�ːi�j
		if (this->attackTime >= 3.0f) {
			this->position.x += vectol.m128_f32[0] / 25;
			this->position.y += vectol.m128_f32[1] / 25;
			this->position.z += vectol.m128_f32[2] / 25;
		}
		else if (this->attackTime >= 1.0f) {
			this->position.x -= this->vectol.m128_f32[0] / 100;
			this->position.y -= this->vectol.m128_f32[1] / 100;
			this->position.z -= this->vectol.m128_f32[2] / 100;
		}

		//�ړ������ڍs���̏�����
		if (this->attackTime >= 5.0f) {
			moveReset();
			this->attackTime = 0.0f;
			actionPattern = MoveMode::move;
			this->startAttack = false;
			this->attackHit = true;
		}

		this->attackTime += 1.0f / 60.0f;
	}
	if (this->mode == 3) {
		//�U�����̏��擾
		if (this->startAttack == false) {
			moveReset();
			this->vectol = objectVector(gPos, this->position);
			this->attackPos = this->position;
			this->startAttack = true;
		}

		//�U���J�n�i�ːi�j
		if (this->attackTime >= 3.0f) {
			if (this->attackHit == true) {
				this->position.x += this->vectol.m128_f32[0] / 25;
				this->position.y += this->vectol.m128_f32[1] / 25;
				this->position.z += this->vectol.m128_f32[2] / 25;
				this->attackOnMove = true;
			}
			else {
				move(Normalize(objectVector(this->position, this->attackPos)));
				this->matRot = LookAtRotation(
					VectorToXMFloat(Normalize(objectVector(this->position, this->attackPos))),
					XMFLOAT3(0.0f, 1.0f, 0.0f));
			}
		}
		else if (this->attackTime >= 1.0f) {
			this->position.x -= this->vectol.m128_f32[0] / 100;
			this->position.y -= this->vectol.m128_f32[1] / 100;
			this->position.z -= this->vectol.m128_f32[2] / 100;
		}

		//�ړ������ڍs���̏�����
		if (attackTime >= 5.0f && this->attackOnMove == true) {
			moveReset();
			this->attackTime = 0.0f;
			actionPattern = MoveMode::move;
			this->startAttack = false;
			this->attackHit = true;
		}
		this->attackTime += 1.0f / 60.0f;
	}
}

void Enemy::Retreat()
{
	if (actionPattern != MoveMode::retreat)return;

	move(Normalize(objectVector(position, RetreatPos)));

	matRot = LookAtRotation(
		VectorToXMFloat(Normalize(objectVector(position, RetreatPos))),
		XMFLOAT3(0.0f, 1.0f, 0.0f));

	if (alpha <= 0) { alive = false; }

	alpha -= 0.01f;
}

void Enemy::Damage()
{
	
	static float count = 0.0f;
	for (int i = 0; i < model->GetMesh().size(); i++) {
		this->model->GetMesh()[i]->GetMaterial()->ambient.x = defalt_ambient[i].x * 2.0f;
		this->model->GetMesh()[i]->GetMaterial()->Update();
	}
	damage = false;
	count += 1.0f / 20.0f;
	if (count >= 1.0f) {
		for (int i = 0; i < model->GetMesh().size(); i++) {
			this->model->GetMesh()[i]->GetMaterial()->ambient.x = defalt_ambient[i].x;
			this->model->GetMesh()[i]->GetMaterial()->Update();
		}
		count = 0.0f;
	}

	if (status.HP <= 0) { actionPattern = MoveMode::retreat; }
}

void Enemy::moveUpdate(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{

	Move(pPos, bPos, gPos);

	Attack(pPos, bPos, gPos);

	Retreat();

	Update();
}
void Enemy::moveReset()
{
}

