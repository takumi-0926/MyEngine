#include "Player.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

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

Player* Player::Create(PMDmodel* _model)
{
	//�C���X�^���X����
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	//������
	if (!instance->Initialize(_model)) {
		delete instance;
		assert(0);
	}

	return instance;
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

	//Z�����x�N�g��
	Zv = { 0.0f,0.0f,0.5f,0.0f };

	//�p�x��]
	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	//Z�����x�N�g������]
	Zv = XMVector3TransformNormal(Zv, matRot);

	//���Z
	pos.x += Zv.m128_f32[0] * directInput->getLeftY() * speed;
	pos.y += Zv.m128_f32[1] * directInput->getLeftY() * speed;
	pos.z += Zv.m128_f32[2] * directInput->getLeftY() * speed;

	return pos;
}

void Player::Update()
{
	if (model->position.x <= -100.0f) {
		model->position.x = -100.0f;
	}
	if (model->position.x >= 100.0f) {
		model->position.x = 100.0f;
	}
	if (model->position.z <= -100.0f) {
		model->position.z = -100.0f;
	}
	if (model->position.z >= 322.0f) {
		model->position.z = 322.0f;
	}

	//�ړ�
	//{
	//	//�ړ��x�N�g��
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
	//		//���ړ�
	//		if (input.Push(DIK_A) || directInput->leftStickX() < 0.0f) {
	//			model->position = (MoveLeft(model->position));
	//		}
	//		//�E�ړ�
	//		if (input.Push(DIK_D) || directInput->leftStickX() > 0.0f) {
	//			model->position = (MoveRight(model->position));
	//		}
	//		//���ړ�
	//		if (input.Push(DIK_W) || directInput->leftStickY() < 0.0f) {
	//			model->position = (MoveBefore(model->position));
	//		}
	//		//��ړ�
	//		if (input.Push(DIK_S) || directInput->leftStickY() > 0.0f) {
	//			model->position = (MoveAfter(model->position));
	//		}
	//		XMMATRIX matRot = XMMatrixIdentity();
	//		//�p�x��]
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

	//����s��
	if (Action == action::Avoid) { Avoid(); }

	//�ړ�
	PMDobject::Update();
}

void Player::Draw()
{
	PMDobject::Draw();
}
