#pragma once
#include "PMD/pmdObject3D.h"
#include "FBX/FbxObject3d.h"
#include "Weapon.h"
#include "hitBox.h"

#include"Input/input.h"
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
class Player : public FbxObject3d {
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

	Weapon* weapon = nullptr;

	float avoidSpeed = 1.0f;
	int Action = 0;

	float avoidTime = 0.0f;
	float speed = 0.0;
	float angleVertical, angleHorizonal;

	//�n�ʔ���p
	bool OnGround = true;
	XMVECTOR fallV;

	int followBoneNum = 0;

private:
	void actionExecution(int num);
	void moveUpdate();
	void Avoid();

	/// <summary>
/// �ړ�
/// </summary>
/// <param name="pos">�ړ���������W</param>
/// <returns>�ړ���̍��W</returns>
	inline XMFLOAT3 MoveBefore(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//Z�����x�N�g��
		Zv = { 0.0f,0.0f,1.0f,0.0f };

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
	inline XMFLOAT3 MoveAfter(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//Z�����x�N�g��
		Zv = { 0.0f,0.0f,1.0f,0.0f };

		//�e�p�x��]
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//Z�����x�N�g������]
		Zv = XMVector3TransformNormal(Zv, matRot);

		//���Z
		pos.x += Zv.m128_f32[0] * directInput->getLeftY() * speed;
		pos.y += Zv.m128_f32[1] * directInput->getLeftY() * speed;
		pos.z += Zv.m128_f32[2] * directInput->getLeftY() * speed;

		return pos;
	}
	inline XMFLOAT3 MoveLeft(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//X�����x�N�g��
		Xv = { 1.0f,0.0f,0.0f,0.0f };

		//�p�x��]
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//X�����x�N�g������]
		Xv = XMVector3TransformNormal(Xv, matRot);

		//���Z
		pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
		pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
		pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;

		return pos;
	}
	inline XMFLOAT3 MoveRight(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//X�����x�N�g��
		Xv = { 1.0f,0.0f,0.0f,0.0f };

		//�p�x��]
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//X�����x�N�g������]
		Xv = XMVector3TransformNormal(Xv, matRot);

		//���Z
		pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
		pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
		pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;

		return pos;
	}

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
	static Player* Create(FbxModel* model);
	/// <summary>
	/// ������
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// �X�V����
	/// </summary>
	void Update()override;
	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList)override;

	void CreateWeapon(Model* model);

public:
	//void SetInput(const Input& input) { this->input = input; }
	void SetAction(int num) { this->Action = num; }
	void SetMoveVec(DirectX::XMFLOAT3 vec) { this->moveVec = vec; }
	void SetAvoidVec(DirectX::XMFLOAT3 vec) { this->avoidVec = vec; }

	int GetAction() { return Action; }

	inline float GetAngleVertical() { return angleVertical; }
	inline float GetAngleHorizonal() { return angleHorizonal; }
};