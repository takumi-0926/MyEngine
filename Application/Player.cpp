#include "Player.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

#include "Collision/CollisionManager.h"
#include "Collision/CollisionAttribute.h"
#include "Collision/SphereCollider.h"

XMMATRIX Player::LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward)
{
	Vector3 z = Vector3(forward.x, forward.y, forward.z);//進行方向ベクトル（前方向）
	Vector3 up = Vector3(upward.x, upward.y, upward.z);  //上方向
	XMMATRIX rot;//回転行列
	Quaternion q = quaternion(0, 0, 0, 1);//回転クォータニオン
	Vector3 _z = { 0.0f,0.0f,1.0f };//Z方向単位ベクトル
	Vector3 cross;
	XMMATRIX matRot = XMMatrixIdentity();

	float a;//角度保存用
	float b;//角度保存用
	float c;//角度保存用
	float d;//角度保存用

	//カメラに合わせるための回転行列
	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	cross = z.cross(_z);

	q.x = cross.x;
	q.y = cross.y;
	q.z = cross.z;

	q.w = sqrt(
		(z.length() * z.length())
		* (_z.length() * _z.length())) + z.dot(_z);

	//単位クォータニオン化
	q = normalize(q);
	q = conjugate(q);
	a = q.x;
	b = q.y;
	c = q.z;
	d = q.w;

	//任意軸回転
	XMVECTOR rq = { q.x,q.y,q.z,q.w };
	rot = XMMatrixRotationQuaternion(rq);

	return rot;
}

Player* Player::Create(FbxModel* model)
{
	//インスタンス生成
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	//初期化
	instance->Initialize();

	instance->SetModel(model);

	return instance;
}

void Player::Initialize()
{
	FbxObject3d::Initialize();

	//コライダー追加
	float radius = 3.0f;
	SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	collider->SetAttribute(COLLISION_ATTR_ALLIES);
}

void Player::actionExecution(int num)
{
	XMFLOAT3 v = { (directInput->getLeftX()),0.0f,-(directInput->getLeftY()) };

	//待機状態
	if (num == action::Wait) {

	}
	//移動状態
	else if (num == action::Walk || num == action::Dash) {

		//左移動
		if (Input::GetInstance()->Push(DIK_A) || directInput->leftStickX() < 0.0f) {
			SetPosition((MoveLeft(GetPosition())));
		}
		//右移動
		if (Input::GetInstance()->Push(DIK_D) || directInput->leftStickX() > 0.0f) {
			SetPosition((MoveRight(GetPosition())));
		}
		//下移動
		if (Input::GetInstance()->Push(DIK_W) || directInput->leftStickY() < 0.0f) {
			SetPosition((MoveBefore(GetPosition())));
		}
		//上移動
		if (Input::GetInstance()->Push(DIK_S) || directInput->leftStickY() > 0.0f) {
			SetPosition((MoveAfter(GetPosition())));
		}
		XMMATRIX matRot = XMMatrixIdentity();
		//角度回転
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		XMVECTOR _v({ v.x, v.y, v.z, 0 });
		_v = XMVector3TransformNormal(_v, matRot);
		v.x = _v.m128_f32[0];
		v.y = _v.m128_f32[1];
		v.z = _v.m128_f32[2];

		SetMatRot(LookAtRotation(v, XMFLOAT3(0.0f, 1.0f, 0.0f)));
	}
	//攻撃状態
	else if (num == action::Attack) {
		if (playEnd) {
			Action = -1;
		}
	}
	playEnd = false;
}

void Player::moveUpdate()
{
	//攻撃状態の時はスキップ（変更しない）
	if (Action == action::Attack) {
		actionExecution(Action);
		return;
	}
	//回避状態の時はスキップ（変更しない）
	if (Action == action::Avoid) {
		actionExecution(Action);
		return;
	}

	if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonB) || Input::GetInstance()->Push(DIK_SPACE)) {
		ChangeAnimation(action::Avoid);
		Action = action::Avoid;

	}
	else if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonX) || Input::GetInstance()->Push(DIK_X)) {
		ChangeAnimation(action::Attack);
		Action = action::Attack;

		//武器の当たり判定有効化
		weapon->SetColliderInvisible(false);
	}
	else if (directInput->leftStickX() < 0.0f || directInput->leftStickX() > 0.0f || directInput->leftStickY() < 0.0f || directInput->leftStickY() > 0.0f) {

		//走りとダッシュの切り替え
		if (directInput->getTriggerZ() != 0) {
			speed = 2.0f;
			ChangeAnimation(action::Dash);
			Action = action::Dash;
		}
		else {
			speed = 1.0f;
			ChangeAnimation(action::Walk);
			Action = action::Walk;
		}
	}
	else {
		ChangeAnimation(action::Wait);
		Action = action::Wait;
	}

	float angleH = 150.0f;
	float angleV = 60.0f;

	if (directInput->rightStickX() >= 0.5f || directInput->rightStickX() <= -0.5f) {
		angleHorizonal +=
			XMConvertToRadians(angleH * directInput->getRightX());
	}
	if (directInput->rightStickY() >= 0.5f || directInput->rightStickY() <= -0.5f) {
		angleVertical +=
			XMConvertToRadians(angleV * directInput->getRightY());
		//制限角度
		if (angleVertical >= 60) {
			angleVertical = 60;
		}
		//制限角度
		if (angleVertical <= -60) {
			angleVertical = -60;
		}
	}

	//行動実行
	actionExecution(Action);
}

void Player::Avoid() {
	if (avoidTime >= 36.0f) {
		Action = action::Wait;
		avoidTime = 0.0f;
	}

	XMVECTOR vec = XMLoadFloat3(&avoidVec);
	vec = XMVector3Normalize(vec);
	position.x += vec.m128_f32[0] * avoidSpeed;
	position.y += vec.m128_f32[1] * avoidSpeed;
	position.z += vec.m128_f32[2] * avoidSpeed;

	avoidTime += 1.0f;
}
void Player::CreateWeapon(Model* model)
{
	weapon = Weapon::Create(model);
	followBoneNum = 42;
}

void Player::Update()
{
	moveUpdate();

	//武器にボーン行列を渡す
	weapon->SetFollowingObjectBoneMatrix(
		model->GetBones()[followBoneNum].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime),matWorld);
	weapon->Update();

	//回避行動
	if (Action == action::Avoid) { Avoid(); }

	//落下処理
	if (!OnGround) {
		const float fallAcc = -0.1f;
		const float fallVYMin = -0.5f;

		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);

		position.x += fallV.m128_f32[0];
		position.y += fallV.m128_f32[1];
		position.z += fallV.m128_f32[2];
	}

	//球コライダー生成
	SphereCollider* sqhereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sqhereCollider);

	//専用クエリーコールバッククラス宣言
	class PlayerQueryCallBack :public QueryCallBack {
	public:
		PlayerQueryCallBack(Sqhere* sphere) :sphere(sphere) {};

		/// <summary>
		/// 衝突時クエリーコールバック関数
		/// </summary>
		bool OnQueryHit(const QueryHit& info) {
			const XMVECTOR up = { 0,1,0,0 };
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			const float threshold = cosf(XMConvertToRadians(30.0f));

			if (-threshold < cos && cos < threshold) {
				sphere->center += info.reject;
				move += info.reject;
			}
			return true;
		}

		Sqhere* sphere = nullptr;
		XMVECTOR move = {};
	};

	PlayerQueryCallBack callBack(sqhereCollider);

	CollisionManager::GetInstance()->QuerySqhere(*sqhereCollider, &callBack, COLLISION_ATTR_LANDSHAPE);

	position.x += callBack.move.m128_f32[0];
	position.y += callBack.move.m128_f32[1];
	position.z += callBack.move.m128_f32[2];
	UpdateWorldMatrix();
	collider->Update();

	//落下判定用レイ
	Ray ray;
	ray.start = sqhereCollider->center;
	ray.start.m128_f32[1] += sqhereCollider->GetRadius();
	ray.dir = { 0,-1,0,0 };
	RaycastHit raycastHit;

	//接地判定
	if (OnGround) {
		const float adsDistance = 0.2f;

		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sqhereCollider->GetRadius() * 2.0f + adsDistance))
		{
			OnGround = true;
			position.y -= (raycastHit.distance - sqhereCollider->GetRadius() * 2.0f);
		}
		else {
			OnGround = false;
			fallV = {};
		}
	}
	else if (fallV.m128_f32[1] <= 0.0f) {
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sqhereCollider->GetRadius() * 2.0f)) {
			OnGround = true;
			position.y -= (raycastHit.distance - sqhereCollider->GetRadius() * 2.0f);
		}
	}
	//移動
	FbxObject3d::Update();
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	FbxObject3d::Draw(cmdList);
	weapon->Draw();
}