#include "Player.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/MyMath.h"
#include "Collision/CollisionManager.h"
#include "Collision/CollisionAttribute.h"
#include "Collision/SphereCollider.h"
#include <2d/PostEffect.h>

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

	SetFastTime(2);

	//コライダー追加（斥候用）
	float radius = 3.0f;
	SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	collider->SetAttribute(COLLISION_ATTR_ALLIES);

	collision.radius = 10.0f;
}

void Player::actionExecution(int num)
{
	XMFLOAT3 v = { (directInput->getLeftX()),0.0f,-(directInput->getLeftY()) };

	//待機状態
	if (num == action::Wait) {

	}
	//移動状態
	else if (num == action::Walk || num == action::Dash || num == action::Avoid) {

		if (num == action::Avoid) {
			Avoid(v);
		}
		else {
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

			_v = { v.x, v.y, v.z, 0 };
			_v = XMVector3TransformNormal(_v, matRot);
			v = XMFLOAT3(_v.m128_f32);

			SetMatRot(LookAtRotation(v, XMFLOAT3(0.0f, 1.0f, 0.0f)));
		}
	}
	//攻撃状態
	else if (num == action::Attack) {
		//if (playEnd) {
		//	Action = -1;
		//}
	}
	//攻撃
	Attack();

	Damage();

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
		attack = true;

		ChangeAnimation(attackNum);
		//武器の当たり判定有効化
		weapon->SetColliderInvisible(false);

		Action = action::Attack;
	}
	else if (directInput->leftStickX() < 0.0f || directInput->leftStickX() > 0.0f || directInput->leftStickY() < 0.0f || directInput->leftStickY() > 0.0f) {

		//走りとダッシュの切り替え
		if (directInput->getTriggerZ() != 0) {
			speed = 3.0f;
			ChangeAnimation(action::Dash);
			Action = action::Dash;
		}
		else {
			speed = 2.0f;
			ChangeAnimation(action::Walk);
			Action = action::Walk;
		}
	}
	else {
		ChangeAnimation(action::Wait);
		Action = action::Wait;
	}

	const float angleH = 150.0f;
	const float angleV = 90.0f;

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

	//攻撃が当たったとき
	if (Hit) {
		if (playEnd) {
			Hit = false;
		}
	}

	//攻撃を受けたとき

	//行動実行
	actionExecution(Action);
}

void Player::Attack()
{
	if (!attack) {
		return;
	}

	if (stTime >= stMax) {
		attackSt = true;
	}

	stTime += fps;

	//コンボアタック先行入力
	if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonX) || Input::GetInstance()->Push(DIK_X)) {

		if (attackNum == action::Attack) { combo = action::Attack2; }
		else if (attackNum == action::Attack2) { combo = action::Attack3; }

		atCombo = true;
	}
	//回避先行入力
	else if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonB) || Input::GetInstance()->Push(DIK_SPACE)) {

		combo = action::Avoid;
	}

	//1秒間先行判定
	if (atCombo) {
		freamCount += fps;

		if (freamCount >= 0.3f) {
			combo = attackNum;
			freamCount = 0.f;
			atCombo = false;
		}
	}

	//行動終了時
	if (playEnd) {

		//攻撃可能
		Hit = false;

		//先行入力確認
		if (atCombo) {

			//入力あり
			if (combo == action::Avoid) {
				ChangeAnimation(action::Avoid);
				Action = action::Avoid;

				attackNum = action::Attack;
				attack = false;

				attackSt = false;
				stTime = {};
			}
			else {

				attackNum = combo;
				atCombo = false;

				attackSt = false;
				stTime = {};

				ChangeAnimation(attackNum);
			}
		}
		else {
			//入力なし
			Action = -1;
			attackNum = action::Attack;
			attack = false;

			attackSt = false;
			stTime = {};
		}

		freamCount = 0.f;
	}
}

void Player::Damage()
{
	if (!damage)return;

	static int count = 10;

	if (count >= 0) {
		status.HP--;
		count--;

		XMVECTOR _vec = twoPointVector(position, damageVec);
		_vec = XMVector3Normalize(_vec);

		position += XMFLOAT3(_vec.m128_f32) * 3;

		PostEffect::mosaic = 1;
	}
	else {
		count = 10;
		damage = false;

		PostEffect::mosaic = 0;
	}
}

void Player::Avoid(const XMFLOAT3& vec) {
	if (avoidTime >= 18.0f) {
		Action = action::Wait;
		avoidTime = 0.0f;
		avoid = false;

		return;
	}

	if (!avoid) {
		avoidVec = vec;
		avoid = true;
	}

	XMMATRIX matRot = XMMatrixIdentity();
	//角度回転
	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	XMVECTOR _vec = XMLoadFloat3(&avoidVec);
	_vec = XMVector3Normalize(_vec);
	_vec = XMVector3TransformNormal(_vec, matRot);

	position -= XMFLOAT3(_vec.m128_f32) * 3;
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

	collision.center = XMLoadFloat3(&position);

	weapon->SetCollision(XMVectorSet(position.x + _v.m128_f32[0], position.y, position.z + _v.m128_f32[2], 1));

	//武器にボーン行列を渡す
	weapon->SetFollowingObjectBoneMatrix(
		model->GetBones()[followBoneNum].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime), matWorld);
	weapon->Update();

	//落下処理
	if (!OnGround) {
		const float fallAcc = -0.1f;
		const float fallVYMin = -0.5f;

		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);

		position += XMFLOAT3(fallV.m128_f32);
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

	position += XMFLOAT3(callBack.move.m128_f32);

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

	if (Action == action::Attack) {
		weapon->Draw();
	}
}