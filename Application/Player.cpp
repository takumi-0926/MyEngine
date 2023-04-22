#include "Player.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

#include "Collision/CollisionManager.h"
#include "Collision/CollisionAttribute.h"
#include "Collision/SphereCollider.h"

XMMATRIX Player::LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward)
{
	Vector3 z = Vector3(forward.x, forward.y, forward.z);//�i�s�����x�N�g���i�O�����j
	Vector3 up = Vector3(upward.x, upward.y, upward.z);  //�����
	XMMATRIX rot;//��]�s��
	Quaternion q = quaternion(0, 0, 0, 1);//��]�N�H�[�^�j�I��
	Vector3 _z = { 0.0f,0.0f,1.0f };//Z�����P�ʃx�N�g��
	Vector3 cross;
	XMMATRIX matRot = XMMatrixIdentity();

	float a;//�p�x�ۑ��p
	float b;//�p�x�ۑ��p
	float c;//�p�x�ۑ��p
	float d;//�p�x�ۑ��p

	//�J�����ɍ��킹�邽�߂̉�]�s��
	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

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

	return rot;
}

Player* Player::Create(FbxModel* model)
{
	//�C���X�^���X����
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	//������
	instance->Initialize();

	instance->SetModel(model);

	return instance;
}

void Player::Initialize()
{
	FbxObject3d::Initialize();

	//�R���C�_�[�ǉ�
	float radius = 3.0f;
	SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));
	collider->SetAttribute(COLLISION_ATTR_ALLIES);
}

void Player::actionExecution(int num)
{
	XMFLOAT3 v = { (directInput->getLeftX()),0.0f,-(directInput->getLeftY()) };

	//�ҋ@���
	if (num == action::Wait) {

	}
	//�ړ����
	else if (num == action::Walk || num == action::Dash) {

		//���ړ�
		if (Input::GetInstance()->Push(DIK_A) || directInput->leftStickX() < 0.0f) {
			SetPosition((MoveLeft(GetPosition())));
		}
		//�E�ړ�
		if (Input::GetInstance()->Push(DIK_D) || directInput->leftStickX() > 0.0f) {
			SetPosition((MoveRight(GetPosition())));
		}
		//���ړ�
		if (Input::GetInstance()->Push(DIK_W) || directInput->leftStickY() < 0.0f) {
			SetPosition((MoveBefore(GetPosition())));
		}
		//��ړ�
		if (Input::GetInstance()->Push(DIK_S) || directInput->leftStickY() > 0.0f) {
			SetPosition((MoveAfter(GetPosition())));
		}
		XMMATRIX matRot = XMMatrixIdentity();
		//�p�x��]
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		XMVECTOR _v({ v.x, v.y, v.z, 0 });
		_v = XMVector3TransformNormal(_v, matRot);
		v.x = _v.m128_f32[0];
		v.y = _v.m128_f32[1];
		v.z = _v.m128_f32[2];

		SetMatRot(LookAtRotation(v, XMFLOAT3(0.0f, 1.0f, 0.0f)));
	}
	//�U�����
	else if (num == action::Attack) {
		if (playEnd) {
			Action = -1;
		}
	}
	playEnd = false;
}

void Player::moveUpdate()
{
	//�U����Ԃ̎��̓X�L�b�v�i�ύX���Ȃ��j
	if (Action == action::Attack) {
		actionExecution(Action);
		return;
	}
	//�����Ԃ̎��̓X�L�b�v�i�ύX���Ȃ��j
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

		//����̓����蔻��L����
		weapon->SetColliderInvisible(false);
	}
	else if (directInput->leftStickX() < 0.0f || directInput->leftStickX() > 0.0f || directInput->leftStickY() < 0.0f || directInput->leftStickY() > 0.0f) {

		//����ƃ_�b�V���̐؂�ւ�
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
		//�����p�x
		if (angleVertical >= 60) {
			angleVertical = 60;
		}
		//�����p�x
		if (angleVertical <= -60) {
			angleVertical = -60;
		}
	}

	//�s�����s
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

	//����Ƀ{�[���s���n��
	weapon->SetFollowingObjectBoneMatrix(
		model->GetBones()[followBoneNum].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime),matWorld);
	weapon->Update();

	//����s��
	if (Action == action::Avoid) { Avoid(); }

	//��������
	if (!OnGround) {
		const float fallAcc = -0.1f;
		const float fallVYMin = -0.5f;

		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);

		position.x += fallV.m128_f32[0];
		position.y += fallV.m128_f32[1];
		position.z += fallV.m128_f32[2];
	}

	//���R���C�_�[����
	SphereCollider* sqhereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sqhereCollider);

	//��p�N�G���[�R�[���o�b�N�N���X�錾
	class PlayerQueryCallBack :public QueryCallBack {
	public:
		PlayerQueryCallBack(Sqhere* sphere) :sphere(sphere) {};

		/// <summary>
		/// �Փˎ��N�G���[�R�[���o�b�N�֐�
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

	//��������p���C
	Ray ray;
	ray.start = sqhereCollider->center;
	ray.start.m128_f32[1] += sqhereCollider->GetRadius();
	ray.dir = { 0,-1,0,0 };
	RaycastHit raycastHit;

	//�ڒn����
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
	//�ړ�
	FbxObject3d::Update();
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	FbxObject3d::Draw(cmdList);
	weapon->Draw();
}