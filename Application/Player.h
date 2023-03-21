#pragma once
#include "PMD/pmdObject3D.h"
//#include "Input/input.h"

enum action {
	Wait = 0,
	Walk,
	Dash,
	Attack,
	Damage,
	Avoid,
};

//class Input;
class Player : public PMDobject {
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	//Input input;

private:
	/// <summary>
	/// �X�e�[�^�X
	/// </summary>
	struct Status {
		float HP;
		float Attack;
	};

	DirectX::XMFLOAT3 moveVec = {};
	DirectX::XMFLOAT3 avoidVec = {};
	XMVECTOR Zv, Xv, Yv;

	float avoidSpeed = 1.0f;
	int Action = 0;

	float avoidTime = 0.0f;
	float speed = 0.0;
	float angleVertical, angleHorizonal;

	//�n�ʔ���p
	bool OnGround = true;
	XMVECTOR fallV;

private:
	void Avoid();

	/// <summary>
	/// �ړ�
	/// </summary>
	/// <param name="pos">�ړ���������W</param>
	/// <returns>�ړ���̍��W</returns>
	//XMFLOAT3 MoveBefore(XMFLOAT3 pos);
	//XMFLOAT3 MoveAfter(XMFLOAT3 pos)
	//{
	//	XMMATRIX matRot = XMMatrixIdentity();

	//	//Z�����x�N�g��
	//	Zv = { 0.0f,0.0f,0.5f,0.0f };

	//	//�e�p�x��]
	//	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	//	//Z�����x�N�g������]
	//	Zv = XMVector3TransformNormal(Zv, matRot);

	//	//���Z
	//	pos.x += Zv.m128_f32[0] * directInput->getLeftY() * speed;
	//	pos.y += Zv.m128_f32[1] * directInput->getLeftY() * speed;
	//	pos.z += Zv.m128_f32[2] * directInput->getLeftY() * speed;

	//	return pos;
	//}
	//XMFLOAT3 MoveLeft(XMFLOAT3 pos)
	//{
	//	XMMATRIX matRot = XMMatrixIdentity();

	//	//X�����x�N�g��
	//	Xv = { 0.5f,0.0f,0.0f,0.0f };

	//	//�p�x��]
	//	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	//	//X�����x�N�g������]
	//	Xv = XMVector3TransformNormal(Xv, matRot);

	//	//���Z
	//	pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
	//	pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
	//	pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;

	//	return pos;
	//}
	//XMFLOAT3 MoveRight(XMFLOAT3 pos)
	//{
	//	XMMATRIX matRot = XMMatrixIdentity();

	//	//X�����x�N�g��
	//	Xv = { 0.5f,0.0f,0.0f,0.0f };

	//	//�p�x��]
	//	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	//	//X�����x�N�g������]
	//	Xv = XMVector3TransformNormal(Xv, matRot);

	//	//���Z
	//	pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
	//	pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
	//	pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;

	//	return pos;
	//}

	/// <summary>
	/// �i�s�����ɉ�]
	/// </summary>
	/// <param name="forward">�i�s�����x�N�g��</param>
	/// <param name="upward">��x�N�g��</param>
	/// <returns>��]�s��i�N�H�[�^�j�I���j</returns>
	XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward);

public:
	/// <summary>
	/// �C���X�^���X����
	/// </summary>
	/// <param name="_model"></param>
	/// <returns></returns>
	static Player* Create(PMDmodel* _model);
	/// <summary>
	/// ������
	/// </summary>
	bool Initialize(PMDmodel* _model)override;
	/// <summary>
	/// �X�V����
	/// </summary>
	void Update()override;
	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw(bool isShadow = false)override;

public:
	//void SetInput(const Input& input) { this->input = input; }
	void SetAction(int num) { this->Action = num; }
	void SetMoveVec(DirectX::XMFLOAT3 vec) { this->moveVec = vec; }
	void SetAvoidVec(DirectX::XMFLOAT3 vec) { this->avoidVec = vec; }

	int GetAction() { return Action; }
};