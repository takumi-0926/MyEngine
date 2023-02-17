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

Player* Player::Create(PMDmodel* _model)
{
	//インスタンス生成
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(_model)) {
		delete instance;
		assert(0);
	}

	return instance;
}

bool Player::Initialize(PMDmodel* _model)
{
	if (!PMDobject::Initialize(_model)) {
		assert(0);
		return false;
	}

	//コライダー追加
	float radius = 3.0f;
	SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	collider->SetAttribute(COLLISION_ATTR_ALLIES);

	return true;
}

void Player::Avoid() {
	if (avoidTime >= 30.0f) {
		Action = action::Wait;
		avoidTime = 0.0f;
	}

	XMVECTOR vec = XMLoadFloat3(&avoidVec);
	vec = XMVector3Normalize(vec);
	model->position.x += -vec.m128_f32[0] * avoidSpeed;
	model->position.y += -vec.m128_f32[1] * avoidSpeed;
	model->position.z += -vec.m128_f32[2] * avoidSpeed;

	avoidTime += 1.0f;
}
XMFLOAT3 Player::MoveBefore(XMFLOAT3 pos)
{
	XMMATRIX matRot = XMMatrixIdentity();

	//Z方向ベクトル
	Zv = { 0.0f,0.0f,0.5f,0.0f };

	//角度回転
	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	//Z方向ベクトルを回転
	Zv = XMVector3TransformNormal(Zv, matRot);

	//加算
	pos.x += Zv.m128_f32[0] * directInput->getLeftY() * speed;
	pos.y += Zv.m128_f32[1] * directInput->getLeftY() * speed;
	pos.z += Zv.m128_f32[2] * directInput->getLeftY() * speed;

	return pos;
}

void Player::Update()
{
	//if (model->position.x <= -100.0f) {
	//	model->position.x = -100.0f;
	//}
	//if (model->position.x >= 100.0f) {
	//	model->position.x = 100.0f;
	//}
	//if (model->position.z <= -100.0f) {
	//	model->position.z = -100.0f;
	//}
	//if (model->position.z >= 322.0f) {
	//	model->position.z = 322.0f;
	//}

	//移動
	//{
	//	//移動ベクトル
	//	XMFLOAT3 v = { (directInput->getLeftX()),0.0f,-(directInput->getLeftY()) };

	//	if (model->oldVmdNumber != vmdData::ATTACK) { model->oldVmdNumber = model->vmdNumber; }
	//	else if (model->oldVmdNumber != vmdData::DAMAGE) { model->oldVmdNumber = model->vmdNumber; }
	//	if (directInput->leftStickX() < 0.0f || directInput->leftStickX() > 0.0f || directInput->leftStickY() < 0.0f || directInput->leftStickY() > 0.0f) {
	//		model->vmdNumber = vmdData::WALK;
	//		if (GetAction() == action::Avoid) { model->vmdNumber = vmdData::AVOID; }
	//		if (directInput->getTriggerZ() != 0) {
	//			speed = 2.0f;
	//		}
	//		else { speed = 1.0f; }
	//		//左移動
	//		if (input.Push(DIK_A) || directInput->leftStickX() < 0.0f) {
	//			model->position = (MoveLeft(model->position));
	//		}
	//		//右移動
	//		if (input.Push(DIK_D) || directInput->leftStickX() > 0.0f) {
	//			model->position = (MoveRight(model->position));
	//		}
	//		//下移動
	//		if (input.Push(DIK_W) || directInput->leftStickY() < 0.0f) {
	//			model->position = (MoveBefore(model->position));
	//		}
	//		//上移動
	//		if (input.Push(DIK_S) || directInput->leftStickY() > 0.0f) {
	//			model->position = (MoveAfter(model->position));
	//		}
	//		XMMATRIX matRot = XMMatrixIdentity();
	//		//角度回転
	//		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	//		XMVECTOR _v({ v.x, v.y, v.z, 0 });
	//		_v = XMVector3TransformNormal(_v, matRot);
	//		v.x = _v.m128_f32[0];
	//		v.y = _v.m128_f32[1];
	//		v.z = _v.m128_f32[2];

	//		model->SetMatRot(LookAtRotation(v, XMFLOAT3(0.0f, 1.0f, 0.0f)));
	//		if (directInput->IsButtonPush(DirectInput::ButtonKind::Button02) || input.Push(DIK_Z)) {
	//			model->vmdNumber = vmdData::AVOID;
	//			SetAction(action::Avoid);
	//			SetAvoidVec(v);
	//		}

	//	}
	//	else if (directInput->IsButtonPush(DirectInput::ButtonKind::Button01) || input.Push(DIK_X)) {
	//		model->vmdNumber = vmdData::ATTACK;
	//	}
	//	else {
	//		model->vmdNumber = vmdData::WAIT;
	//	}
	//}

	//回避行動
	if (Action == action::Avoid) { Avoid(); }

	//落下処理
	if (!OnGround) {
		const float fallAcc = -0.1f;
		const float fallVYMin = -0.5f;

		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);

		model->position.x += fallV.m128_f32[0];
		model->position.y += fallV.m128_f32[1];
		model->position.z += fallV.m128_f32[2];
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

	model->position.x += callBack.move.m128_f32[0];
	model->position.y += callBack.move.m128_f32[1];
	model->position.z += callBack.move.m128_f32[2];
	model->UpdateWorldMatrix();
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
			model->position.y -= (raycastHit.distance - sqhereCollider->GetRadius() * 2.0f);
		}
		else {
			OnGround = false;
			fallV = {};
		}
	}
	else if (fallV.m128_f32[1] <= 0.0f) {
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sqhereCollider->GetRadius() * 2.0f)) {
			OnGround = true;
			model->position.y -= (raycastHit.distance - sqhereCollider->GetRadius() * 2.0f);
		}
	}
	//移動
	PMDobject::Update();
}

void Player::Draw()
{
	PMDobject::Draw();
}
