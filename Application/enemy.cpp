#include "enemy.h"
#include "Collision/CollisionManager.h"
#include "Collision/SphereCollider.h"
#include "Collision/CollisionAttribute.h"
#include "Collision/QueryCallBack.h"

#include "safeDelete.h"
#include <Math/MyMath.h>
#include "Easing.h"

Easing ease;

enum MoveMode {
	Move,
	Attack,
	Avoid,
	Retreat,
};

Enemy::Enemy()
{
	status = {
					   2,//�f�t�H���gHP
				   10.0f,//�f�t�H���g�U����
					1.0f,//�f�t�H���g�h���
					1.0f,//�f�t�H���g���x
		{1.0f,1.0f,1.0f},//�f�t�H���g�傫��
	};
	alive = false;
	mode = 0;
	vectol = {};
	oldPos = {};
	attackPos = {};
}

Enemy::~Enemy()
{
	safe_delete(model);
}

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
void Enemy::Move(XMVECTOR vector)
{
	this->position.x -= vector.m128_f32[0] * this->status.speed;
	this->position.y -= vector.m128_f32[1] * this->status.speed;
	this->position.z -= vector.m128_f32[2] * this->status.speed;
}


unique_ptr<Enemy> Enemy::Create(FbxModel* model, const int Type)
{
	//�C���X�^���X�𐶐�
	unique_ptr<Enemy> instance = make_unique<Enemy>();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->position.y = 20;
	instance->position.z = -70;

	instance->collision.center = XMLoadFloat3(&instance->position);
	instance->collision.radius = 10.0f;

	//�g�p���f���o�^
	instance->model = model;
	instance->model->SetTextureOffset(Type);

	//���ʔԍ��ݒ�
	instance->myNumber = rand() % RAND_MAX;

	instance->mode = Type;
	//�A���r�G���g�����擾
	//instance->defalt_ambient.push_back(model->ambient);

	// ������
	instance->Initialize();
	//���܂ꂽ
	instance->Appearance();

	instance->Particle();

	instance->obb.Create();

	instance->SetFastTime(1);

	return move(instance);
}
void Enemy::Initialize()
{
	FbxObject3d::Initialize();

	float radius = 10.0f;

	SetModel(model);

	SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
}
void Enemy::Update() {

	UpdateWorldMatrix();
	collider->Update();

	particle->Update();

	//obb�X�V
	obb.SetVector(0, XMVector3TransformNormal(XMVECTOR{ 1, 0, 0 }, ExtractRotationMat(matWorld)));
	obb.SetVector(1, XMVector3TransformNormal(XMVECTOR{ 0, 1, 0 }, ExtractRotationMat(matWorld)));
	obb.SetVector(2, XMVector3TransformNormal(XMVECTOR{ 0, 0, 1 }, ExtractRotationMat(matWorld)));

	obb.SetPos(matWorld);

	obb.SetLength(0, 10);
	obb.SetLength(1, 10);
	obb.SetLength(2, 10);

	//�R���W�����ǐ�
	collision.center = XMLoadFloat3(&position);

	//��������
	if (!OnGround) {
		const float fallAcc = -0.01f;
		const float fallVYMin = -0.5f;

		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);

		position += XMFLOAT3(fallV.m128_f32);
	}

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

	position += XMFLOAT3(callback.move.m128_f32);

	UpdateWorldMatrix();

	collider->Update();

	//��������p���C
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0,-1,0,0 };
	RaycastHit raycastHit;

	//�ڒn����
	if (OnGround) {
		const float adsDistance = 0.2f;

		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance))
		{
			OnGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			FbxObject3d::Update();
		}
		else {
			OnGround = false;
			fallV = {};
		}
	}
	else if (fallV.m128_f32[1] <= 0.0f) {
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f)) {
			OnGround = true;
			position.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			FbxObject3d::Update();
		}
	}

	//���S�����瓖���蔻����Ȃ���
	if (!this->alive) {
		CollisionManager::GetInstance()->RemoveCollider(collider);
	}

	FbxObject3d::Update();
}
void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!alive) { return; }
	SetAlpha();

	FbxObject3d::Draw(cmdList);
}

void Enemy::OnCollision(const CollisionInfo& info)
{
	damage = true;
}

void Enemy::Particle()
{
	particle = ParticleManager::Create();
}

void Enemy::Appearance()
{
	//�O�̂܂�
	//if (popTime >= 10.0f) {
	if (mode = Activity::wolf) {
		status.HP = 15;
		status.speed = 1.0f;
		shadowOffset = 1.0f;
		particleOffset = 10.0f;
		scale = XMFLOAT3(0.7f, 0.7f, 0.7f);
	}
	else if (mode = Activity::golem) {
		status.HP = 4;
		status.speed = 0.6f;
		shadowOffset = 1.5f;
		particleOffset = 40.0f;
		scale = XMFLOAT3(0.15f, 0.15f, 0.15f);
	}
	position = XMFLOAT3(0, 0, -135);

	alive = true;

	alpha = 1.0f;

	actionPattern = 0;

	defalt_ambient.clear();
	defalt_ambient.push_back(model->ambient);

	PlayAnimation(MotionType::WalkMotion);
}

void Enemy::Move(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{
	int objectNo = 0;

	if (actionPattern != MoveMode::Move)return;
	ChangeAnimation(MotionType::WalkMotion);

	//�ړ�����
	//�p�^�[��1
	if (mode == -1) {
		//�����ۑ��p
		float distance = 1000;
		for (int i = 0; i < 6; i++)
		{
			//�����𑪒肵�čU���Ώۂ�����
			float dis = objectDistance(position, bPos[i]->position);
			if (distance >= dis) {
				if (bPos[i]->GetAlive() != true) { continue; }
				distance = dis;
				objectNo = i;
			}
		}

		Move(Normalize(objectVector(position, bPos[objectNo]->position)));
		matRot = LookAtRotation(
			VectorToXMFloat(Normalize(objectVector(position, bPos[objectNo]->position))),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		//�U���Ώۂ��Ȃ��Ȃ����ꍇ
		for (int i = 0; i < 6; i++)
		{
			static int Num = 0;
			if (bPos[i]->GetAlive() == false) { Num++; }
			//�s���p�^�[����ω�
			if (Num == 6) {
				mode = 3;
			}
		}

		static int d = 21;

		//�ړ�����U����
		if (objectDistance(position, bPos[objectNo]->position) <= d) {
			actionPattern = MoveMode::Attack;
			attackOnMove = false;
		}
	}
	//�p�^�[��2
	if (mode == Activity::wolf) {
		Move(Normalize(objectVector(position, pPos)));
		matRot = LookAtRotation(
			VectorToXMFloat(Normalize(objectVector(position, pPos))),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		//�ړ�����U����
		if (objectDistance(position, pPos) <= 21) {
			actionPattern = MoveMode::Attack;
			attackPattern = MotionType::AttackMotion_02;
		}
		if (objectDistance(position, pPos) <= 15) {
			actionPattern = MoveMode::Attack;
			attackPattern = MotionType::AttackMotion_01;
		}
		if (objectDistance(position, pPos) <= 8) {
			actionPattern = MoveMode::Attack;
			attackPattern = MotionType::AttackMotion_03;
		}
	}
	//�p�^�[��3
	if (mode == Activity::golem) {
		Move(Normalize(objectVector(position, gPos)));
		matRot = LookAtRotation(
			VectorToXMFloat(Normalize(objectVector(position, gPos))),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		static int dd = 8;

		//�ړ�����U����
		if (objectDistance(position, gPos) <= dd) {
			actionPattern = MoveMode::Attack;
			attackOnMove = false;
		}
	}
}

void Enemy::Attack(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{
	static int d = 7;
	int objectNo = 0;

	if (actionPattern != MoveMode::Attack)return;
	//ChangeAnimation(MotionType::AttackMotion);

	//�U������
	if (mode == -1) {
		//�U�����̏��擾
		if (attack == false) {
			moveReset();
			vectol = objectVector(bPos[objectNo]->position, position);
			attackPos = this->position;
			attack = true;
		}

		if (attackTime >= 3.0f) {
			if (attackHit == true) {
				position += XMFLOAT3(vectol.m128_f32) * 0.25f;
				attackOnMove = true;
			}
			else {
				Move(Normalize(objectVector(position, bPos[objectNo]->position)));
				matRot = LookAtRotation(
					VectorToXMFloat(Normalize(objectVector(position, gPos))),
					XMFLOAT3(0.0f, 1.0f, 0.0f));

			}
		}
		else if (attackTime >= 1.0f) {
			position -= XMFLOAT3(vectol.m128_f32) * 0.01f;
		}

		//�ړ������ڍs���̏�����
		if (position.x == attackPos.x && attackOnMove == true) {
			if (position.y == attackPos.y) {
				if (position.z == attackPos.z) {
					moveReset();
					attackTime = 0.0f;
					actionPattern = MoveMode::Move;
					attack = false;
					attackHit = false;
				};
			};
		}

		attackTime += fps;
	}
	if (mode == Activity::wolf) {

		//�ːi�U��
		if (attackPattern == MotionType::AttackMotion_01) {

			//�U�����̏��擾
			if (!attack) {
				vectol = objectVector(pPos, position);
				attackPos = position;
				attack = true;
			}

			//�U���J�n�i�ːi�j
			if (attackTime >= 0.5f) {
				position += XMFLOAT3(XMVector3Normalize(vectol).m128_f32) * 4;

				particle->CreateParticle(60,
					XMFLOAT3(position),
					0.01f, 0.02f, 8, 4.0f, { 0.2f,0.2f,0.8f,1 }, 2);

				ChangeAnimation(AttackType_Wolf::Type01_Walk);

				if (attackHit) {
					position -= XMFLOAT3(XMVector3Normalize(vectol).m128_f32) * 2;
				}

			}
			else {
				ChangeAnimation(AttackType_Wolf::Type02_JumpWait, true);
			}

			//�ړ������ڍs���̏�����
			if (attackTime >= 1.0f) {
				moveReset();
				attackTime = 0.0f;
				actionPattern = MoveMode::Move;
				attack = false;
				attackHit = false;
			}

			attackTime += fps;
		}
		//�W�����v�U��
		else if (attackPattern == MotionType::AttackMotion_02) {
			JumpAttack(pPos);
		}
		else if (attackPattern == MotionType::AttackMotion_03) {
			FingerAttack(pPos);
		}
	}
	if (mode == Activity::golem) {
		//�U�����̏��擾
		if (attack) {
			moveReset();
			vectol = objectVector(gPos, position);
			attackPos = position;
			attack = true;
		}

		ChangeAnimation(MotionType::AttackMotion_01);

		//�U���J�n�i�ːi�j
		//if (this->attackTime >= 3.0f) {
		//	if (this->attackHit == true) {
		//		this->position.x += this->vectol.m128_f32[0] / 25;
		//		this->position.y += this->vectol.m128_f32[1] / 25;
		//		this->position.z += this->vectol.m128_f32[2] / 25;
		//		this->attackOnMove = true;
		//	}
		//	else {
		//		move(Normalize(objectVector(this->position, this->attackPos)));
		//		this->matRot = LookAtRotation(
		//			VectorToXMFloat(Normalize(objectVector(this->position, this->attackPos))),
		//			XMFLOAT3(0.0f, 1.0f, 0.0f));
		//	}
		//}
		//else if (this->attackTime >= 1.0f) {
		//	this->position.x -= this->vectol.m128_f32[0] / 100;
		//	this->position.y -= this->vectol.m128_f32[1] / 100;
		//	this->position.z -= this->vectol.m128_f32[2] / 100;
		//}

		//�ړ������ڍs���̏�����
		if (attackTime >= 5.0f && attackOnMove) {
			moveReset();
			attackTime = 0.0f;
			actionPattern = MoveMode::Move;
			attack = false;
			attackHit = false;
		}
		attackTime += 1.0f / 60.0f;
	}
}

void Enemy::JumpAttack(XMFLOAT3& targetPosition)
{
	attackTime += 1.0f / 60.0f;
	if (!attack) {
		attack = true;
		attackHit = false;
		jump.pos = Vector3(position.x, position.y, position.z);
		jump.p1 = Vector3(position.x, position.y, position.z);
		jump.p2 = Vector3((position.x + targetPosition.x) / 2.0f, (position.y + targetPosition.y) / 2.0f + 80.0f, (position.z + targetPosition.z) / 2.0f);
		jump.p3 = Vector3(targetPosition.x, targetPosition.y, targetPosition.z);
	}
	static bool p = false;
	static float time = 0;

	if (attackTime >= 1.0f) {

		ChangeAnimation(AttackType_Wolf::Type02_JumpAttack, true);

		//�x�W�G�Ȑ��ɂ��W�����v
		Vector3 a = lerp(jump.p1, jump.p2, jumpTime);
		Vector3 b = lerp(jump.p2, jump.p3, jumpTime);
		jump.pos = lerp(a, b, jumpTime);
		position = XMFLOAT3(jump.pos.x, jump.pos.y, jump.pos.z);
		//�^���֐�clamp
		{
			if (jumpTime < 0) { jumpTime = 0; }
			else if (jumpTime > maxTime) {
				jumpTime = maxTime;
				p = true;
			}
			else if (jumpTime < maxTime) {
				jumpTime += 1.0f / 30.0f;
			}
			else {
				jumpTime = jumpTime;
			}
		}

		if (p) {
			ChangeAnimation(AttackType_Wolf::Type02_JumpWait, true);

			particle->CreateParticle(30,
				XMFLOAT3(position),
				0.01f, 0.1f, 64, 5.0f, { 0.2f,0.2f,0.8f,1 }, 1);

			rotation.y = 5.0f - ease.easeOut(0.0f, 5.0f, ease.timeRate += fps);

			if (ease.timeRate >= 1.0f) { ease.timeRate = 1.0f; }
		}

		//�ړ������ڍs���̏�����
		if (attackTime >= 3.f) {
			actionPattern = MoveMode::Move;
			moveReset();
			attackTime = 0.0f;
			jumpTime = 0.0f;
			rotation.y = 0;
			attack = false;
			attackHit = false;
			p = false;

			ease.timeRate = 0;
		}
	}
	else {
		ChangeAnimation(AttackType_Wolf::Type02_JumpWait, true);
	}
}

void Enemy::FingerAttack(XMFLOAT3& targetPosition)
{
	//�U�����̏��擾
	if (!attack) {
		vectol = objectVector(targetPosition, position);
		attackPos = position;
		attack = true;
		attackHit = false;
	}

	//�U���J�n�i�ːi�j
	if (attackTime >= 0.5f) {
		rotation.y = 3.f;

		//�G�t�F�N�g
		particle->CreateParticle(30,
			XMFLOAT3(position),
			0.01f, 0.1f, 32, 5.0f, { 0.2f,0.2f,0.8f,1 }, 3);

		collision.radius = 50.0f;

		//�A�j���[�V�����؂�ւ�
		ChangeAnimation(AttackType_Wolf::Type01_Walk);
	}
	else {
		//�A�j���[�V�����؂�ւ�
		ChangeAnimation(AttackType_Wolf::Type02_JumpWait, true);
	}

	//�ړ������ڍs���̏�����
	if (attackTime >= 0.75f) {
		moveReset();
		attackTime = 0.0f;
		collision.radius = 10.0f;
		rotation.y = 0.0f;
		actionPattern = MoveMode::Move;
		attack = false;
		attackHit = false;
	}

	attackTime += 1.0f / 60.0f;
}

void Enemy::Retreat()
{
	if (actionPattern != MoveMode::Retreat)return;

	ChangeAnimation(MotionType::WalkMotion);
	//�ړ�
	Move(Normalize(objectVector(position, RetreatPos)));
	//��]
	matRot = LookAtRotation(
		VectorToXMFloat(Normalize(objectVector(position, RetreatPos))),
		XMFLOAT3(0.0f, 1.0f, 0.0f));

	alpha -= 0.01f;

	if (alpha <= 0.0f) {
		alive = false;
	}
}

void Enemy::Damage(XMFLOAT3& targetPosition)
{
	if (!damage) return;
	vectol = objectVector(targetPosition, position);
	vectol = Normalize(vectol);

	position -= XMFLOAT3(vectol.m128_f32) * 3;
	model->ambient.x = 1.0f;
	damageTime += 1.0f / 60.0f;
	SetScale(XMFLOAT3(0.71f, 0.71f, 0.71f));
	if (damageTime >= 0.1f) {
		SetScale(XMFLOAT3(0.7f, 0.7f, 0.7f));

	}
	if (damageTime >= 0.2f) {
		status.HP -= 1;
		model->ambient.x = defalt_ambient[0].x;

		damage = false;
		damageTime = 0.0f;
	}
	//�_���[�W�p�[�e�B�N������
	particle->CreateParticle(
		60, XMFLOAT3(position.x, position.y + particleOffset, position.z),
		0.0001f, 0.05f, 5, 8.0f, { 1,0,0,1 });

	if (status.HP <= 0) { actionPattern = MoveMode::Retreat; }
	else if (status.HP % 5 == 0 && status.HP != 15) {
	}
}

void Enemy::Down(XMFLOAT3& targetPosition)
{
	static XMVECTOR RightVec;
	static float step;
	static float time;
	if (!down) {
		//�v���C���[���������x�N�g�����擾
		vectol = objectVector(targetPosition, position);
		vectol = Normalize(vectol);

		//�x�N�g���ɐ����ȃx�N�g�����擾
		RightVec = rightVec(vectol);

		//���擾����
		down = true;
	}

	if (step <= 1.0f) {

		//�v���C���[�Ɍ������x�N�g�����琂���ȃx�N�g���ɐ��ʂ��ړ�
		XMVECTOR answer = moveVectortoVector(vectol, RightVec, step += 0.5f);

		matRot = LookAtRotation(
			VectorToXMFloat(Normalize(answer)),
			XMFLOAT3(0.0f, 1.0f, 0.0f));
	}
	else {
		time += fps;

	}

	//�_�E���I���
	if (time >= 5.0f) {

		static bool g;
		static float s;
		if (!g) {
			vectol = objectVector(targetPosition, position);
			g = true;
		}

		//�v���C���[�Ɍ������x�N�g�����琂���ȃx�N�g���ɐ��ʂ��ړ�
		XMVECTOR answer = moveVectortoVector(RightVec, vectol, s += 0.5f);

		matRot = LookAtRotation(
			VectorToXMFloat(Normalize(answer)),
			XMFLOAT3(1.0f, 0.0f, 0.0f));

		if (s >= 1.0f) {
			actionPattern = MoveMode::Move;
			down = false;
			time = 0;
			step = 0;
			s = 0;
		}
	}
}

void Enemy::moveUpdate(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{
	//�ړ�
	Move(pPos, bPos, gPos);
	//�U��
	Attack(pPos, bPos, gPos);
	//�ދp
	Retreat();
	//��_���[�W
	Damage(pPos);
	//
	//Down(pPos);
	//�X�V
	Update();
}
void Enemy::moveReset()
{
}

