#include "enemy.h"
#include "Collision/CollisionManager.h"
#include "Collision/SphereCollider.h"
#include "Collision/CollisionAttribute.h"
#include "Collision/QueryCallBack.h"

enum MoveMode {
	move,
	attack,
	avoid,
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
					   2,//デフォルトHP
				   10.0f,//デフォルト攻撃力
					1.0f,//デフォルト防御力
					1.0f,//デフォルト速度
		{1.0f,1.0f,1.0f},//デフォルト大きさ
	};
	alive = false;
	mode = 0;
	vectol = {};
	oldPos = {};
	attackPos = {};
}
Enemy* Enemy::Create(FbxModel* model1, FbxModel* model2)
{
	// 3Dオブジェクトのインスタンスを生成
	Enemy* instance = new Enemy();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->position.y = 20;
	instance->position.z = -70;

	//使用モデル登録
	instance->modelType[0] = model1;
	instance->modelType[1] = model2;

	//識別番号設定
	instance->myNumber = rand() % RAND_MAX;

	//アンビエント元を取得
	//instance->defalt_ambient.push_back(model->ambient);

	// 初期化
	instance->Initialize();
	//if (!instance->Initialize()) {
	//	delete instance;
	//	assert(0);
	//	return nullptr;
	//}

	//if (model) {
	//	FbxModel* _model = model;
	//	instance->SetModel(_model);
	//	instance->LoadAnima();
	//}

	return instance;
}
void Enemy::Initialize()
{
	//if (!FbxObject3d::Initialize()) {
	//	return false;
	//}
	FbxObject3d::Initialize();

	float radius = 10.0f;

	SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
	//return true;
}
void Enemy::Update() {

	UpdateWorldMatrix();
	collider->Update();

	particle->Update();


	//weapon->SetFollowingObjectBoneMatrix(model->GetBones()[followBoneNum].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime));
	//weapon->SetPosition(position);
	//weapon->Update();

	//落下処理
	if (!OnGround) {
		const float fallAcc = -0.01f;
		const float fallVYMin = -0.5f;

		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);

		position.x += fallV.m128_f32[0];
		position.y += fallV.m128_f32[1];
		position.z += fallV.m128_f32[2];
	}

	//球コライダー取得
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);

	/// <summary>
	/// 専用クエリーコールバック
	/// </summary>
	class EnemyQueryCallBack : public QueryCallBack {
	public:
		EnemyQueryCallBack(Sqhere* sphere) :sphere(sphere) {}

		bool OnQueryHit(const QueryHit& info) {
			const XMVECTOR up = { 0,1,0,0 };

			XMVECTOR rejectDir = XMVector3Normalize(info.reject);

			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			const float thrsehold = cosf(XMConvertToRadians(30.0f));
			//角度差によって壁または地面と判定されている場合を除いて
			if (-thrsehold < cos && cos < thrsehold) {
				//球を押し出す
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

	collider->Update();

	//落下判定用レイ
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = { 0,-1,0,0 };
	RaycastHit raycastHit;

	//接地判定
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

	//死亡したら当たり判定をなくす
	if (!this->alive) {
		CollisionManager::GetInstance()->RemoveCollider(collider);
	}

	//if (position.x <= -100.0f) {
	//	position.x = -100.0f;
	//}
	//if (position.x >= 100.0f) {
	//	position.x = 100.0f;
	//}
	//if (position.z <= -120.0f) {
	//	position.z = -120.0f;
	//}
	//if (position.z >= 342.0f) {
	//	position.z = 342.0f;
	//}

	//static bool flag = false;
	//if (position.z >= -100.0f && !flag) {
	//	position.z = -60.0f;
	//	flag = true;
	//}

	FbxObject3d::Update();
}
void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!alive) { return; }
	SetAlpha();

	FbxObject3d::Draw(cmdList);

	//weapon->Draw();
}

void Enemy::OnCollision(const CollisionInfo& info)
{
	damage = true;
}

void Enemy::CreateWeapon(Model* model)
{
	weapon = Weapon::Create(model);
}

void Enemy::Particle()
{
	particle = ParticleManager::Create();
}

void Enemy::Appearance()
{
	//Enemy* ene = nullptr;
	//static float popTime = 0;
	//三体まで
	//if (popTime >= 10.0f) {
	int r = rand() % 10;
	//if (r % 2 == 1) { ene = Enemy::Create(model2); }
	//if (r % 2 != 1) { ene = Enemy::Create(model1); }
	if (r % 2 == 1) {
		mode = Activity::wolf;
		SetModel(modelType[Activity::wolf]);
		//weapon->SetFollowingObjectBoneMatrix(model->GetBones()[23].fbxCluster->GetLink()->EvaluateGlobalTransform());
		status.HP = 2;
		status.speed = 1.0f;
		scale = { 0.5f,0.5f,0.5f };
		shadowOffset = 1.0f;
		particleOffset = 10.0f;
	}
	else if (r % 2 != 1) {
		mode = Activity::golem;
		SetModel(modelType[Activity::golem]);
		status.HP = 4;
		status.speed = 0.4f;
		scale = { 0.1f,0.1f,0.1f };
		shadowOffset = 1.5f;
		particleOffset = 40.0f;
	}
	position = XMFLOAT3(0, 20, -70);

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

	if (actionPattern != MoveMode::move)return;
	ChangeAnimation(MotionType::WalkMotion);

	//移動処理
	//パターン1
	if (this->mode == -1) {
		float distance = 1000;//距離保存用
		for (int i = 0; i < 6; i++)
		{
			//距離を測定して攻撃対象を決定
			float dis = objectDistance(this->position, bPos[i]->position);
			if (distance >= dis) {
				if (bPos[i]->GetAlive() != true) { continue; }
				distance = dis;
				objectNo = i;
			}
		}

		move(Normalize(objectVector(this->position, bPos[objectNo]->position)));
		this->matRot = LookAtRotation(
			VectorToXMFloat(Normalize(objectVector(this->position, bPos[objectNo]->position))),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		//攻撃対象がなくなった場合
		for (int i = 0; i < 6; i++)
		{
			static int Num = 0;
			if (bPos[i]->GetAlive() == false) { Num++; }
			//行動パターンを変化
			if (Num == 6) {
				this->mode = 3;
			}
		}

		static int d = 21;

		//移動から攻撃へ
		if (objectDistance(this->position, bPos[objectNo]->position) <= d) {
			actionPattern = MoveMode::attack;
			this->attackOnMove = false;
		}
	}
	//パターン2
	if (this->mode == Activity::wolf) {
		move(Normalize(objectVector(this->position, pPos)));
		this->matRot = LookAtRotation(
			VectorToXMFloat(Normalize(objectVector(this->position, pPos))),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		//移動から攻撃へ
		if (objectDistance(this->position, pPos) <= 21) {
			actionPattern = MoveMode::attack;
			attackPattern = AttackType::Type02;
		}
		if (objectDistance(this->position, pPos) <= 15) {
			actionPattern = MoveMode::attack;
			attackPattern = AttackType::Type01;
		}
	}
	//パターン3
	if (this->mode == Activity::golem) {
		move(Normalize(objectVector(this->position, gPos)));
		this->matRot = LookAtRotation(
			VectorToXMFloat(Normalize(objectVector(this->position, gPos))),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		static int dd = 8;

		//移動から攻撃へ
		if (objectDistance(this->position, gPos) <= dd) {
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
	ChangeAnimation(MotionType::AttackMotion);

	//攻撃処理
	if (this->mode == -1) {
		//攻撃時の情報取得
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

		//移動処理移行時の初期化
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
	if (this->mode == Activity::wolf) {

		if (attackPattern == AttackType::Type01) {

			//攻撃時の情報取得
			if (this->startAttack == false) {
				this->vectol = objectVector(pPos, this->position);
				this->attackPos = this->position;
				this->startAttack = true;
			}

			//攻撃開始（突進）
			if (this->attackTime >= 0.5f) {
				this->position.x += XMVector3Normalize(vectol).m128_f32[0] * 4;
				this->position.y += XMVector3Normalize(vectol).m128_f32[1] * 4;
				this->position.z += XMVector3Normalize(vectol).m128_f32[2] * 4;

				particle->CreateParticle(60,
					XMFLOAT3(position),
					0.01f, 0.02f, 8, 2.0f, { 0.2f,0.2f,0.8f,1 }, 2);

				ChangeAnimation(MotionType::AttackMotion);
			}
			else { StopAnimation(); }
			//else if (this->attackTime >= 1.0f) {
			//	this->position.x -= this->vectol.m128_f32[0] / 100;
			//	this->position.y -= this->vectol.m128_f32[1] / 100;
			//	this->position.z -= this->vectol.m128_f32[2] / 100;
			//}

			//移動処理移行時の初期化
			if (this->attackTime >= 1.0f) {
				moveReset();
				this->attackTime = 0.0f;
				actionPattern = MoveMode::move;
				this->startAttack = false;
				this->attackHit = true;
			}

			this->attackTime += 1.0f / 60.0f;
		}
		else if (attackPattern == AttackType::Type02) {
			JumpAttack(pPos);
		}
	}
	if (this->mode == Activity::golem) {
		//攻撃時の情報取得
		if (this->startAttack == false) {
			moveReset();
			this->vectol = objectVector(gPos, this->position);
			this->attackPos = this->position;
			this->startAttack = true;
		}

		//攻撃開始（突進）
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

		//移動処理移行時の初期化
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

void Enemy::JumpAttack(XMFLOAT3& targetPosition)
{
	StopAnimation();
	attackTime += 1.0f / 60.0f;
	if (!startAttack) {
		startAttack = true;
		jump.pos = Vector3(position.x, position.y, position.z);
		jump.p1 = Vector3(position.x, position.y, position.z);
		jump.p2 = Vector3((position.x + targetPosition.x) / 2.0f, (position.y + targetPosition.y) / 2.0f + 80.0f, (position.z + targetPosition.z) / 2.0f);
		jump.p3 = Vector3(targetPosition.x, targetPosition.y, targetPosition.z);
	}
	static bool p = false;

	if (attackTime >= 1.0f) {
		//ベジエ曲線によるジャンプ
		Vector3 a = lerp(jump.p1, jump.p2, jumpTime);
		Vector3 b = lerp(jump.p2, jump.p3, jumpTime);
		jump.pos = lerp(a, b, jumpTime);
		position = XMFLOAT3(jump.pos.x, jump.pos.y, jump.pos.z);
		//疑似関数clamp
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

		if (p) {
			particle->CreateParticle(30,
				XMFLOAT3(position),
				0.01f, 0.1f, 64, 5.0f, { 0.2f,0.2f,0.8f,1 }, 1);
			p = false;
		}

		//移動処理移行時の初期化
		if (this->attackTime >= 3.5f) {
			moveReset();
			this->attackTime = 0.0f;
			actionPattern = MoveMode::move;
			this->startAttack = false;
			this->attackHit = true;

			jumpTime = 0.0f;
		}
	}
	else { StopAnimation(); }
}

void Enemy::Retreat()
{
	if (actionPattern != MoveMode::retreat)return;
	ChangeAnimation(MotionType::WalkMotion);

	move(Normalize(objectVector(position, RetreatPos)));

	matRot = LookAtRotation(
		VectorToXMFloat(Normalize(objectVector(position, RetreatPos))),
		XMFLOAT3(0.0f, 1.0f, 0.0f));

	alpha -= 0.01f;

	if (alpha <= 0.0f) {
		alive = false;
	}
}

void Enemy::Damage()
{
	if (!damage) { return; }

	static float count = 0.0f;
	model->ambient.x = 1.0f;
	count += 1.0f / 20.0f;
	if (count >= 1.0f) {
		status.HP -= 1;
		model->ambient.x = defalt_ambient[0].x;
		damage = false;
		count = 0.0f;
	}

	particle->CreateParticle(
		60, XMFLOAT3(position.x, position.y + particleOffset, position.z),
		0.0001f, 0.05f, 5, 8.0f, { 1,0,0,1 });

	if (status.HP <= 0) { actionPattern = MoveMode::retreat; }
}

void Enemy::ChangeAnimation(int num)
{
	if (nowPlayMotion != num) {
		PlayAnimation(num);
		nowPlayMotion = num;
	}
}

void Enemy::moveUpdate(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{
	//移動
	Move(pPos, bPos, gPos);
	//攻撃
	Attack(pPos, bPos, gPos);
	//退却
	Retreat();
	//被ダメージ
	Damage();
	//更新
	Update();
}
void Enemy::moveReset()
{
}

