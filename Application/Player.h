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
	/// ステータス
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

	//地面判定用
	bool OnGround = true;
	XMVECTOR fallV;

private:
	void Avoid();

	/// <summary>
	/// 移動
	/// </summary>
	/// <param name="pos">移動させる座標</param>
	/// <returns>移動後の座標</returns>
	//XMFLOAT3 MoveBefore(XMFLOAT3 pos);
	//XMFLOAT3 MoveAfter(XMFLOAT3 pos)
	//{
	//	XMMATRIX matRot = XMMatrixIdentity();

	//	//Z方向ベクトル
	//	Zv = { 0.0f,0.0f,0.5f,0.0f };

	//	//弾角度回転
	//	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	//	//Z方向ベクトルを回転
	//	Zv = XMVector3TransformNormal(Zv, matRot);

	//	//加算
	//	pos.x += Zv.m128_f32[0] * directInput->getLeftY() * speed;
	//	pos.y += Zv.m128_f32[1] * directInput->getLeftY() * speed;
	//	pos.z += Zv.m128_f32[2] * directInput->getLeftY() * speed;

	//	return pos;
	//}
	//XMFLOAT3 MoveLeft(XMFLOAT3 pos)
	//{
	//	XMMATRIX matRot = XMMatrixIdentity();

	//	//X方向ベクトル
	//	Xv = { 0.5f,0.0f,0.0f,0.0f };

	//	//角度回転
	//	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	//	//X方向ベクトルを回転
	//	Xv = XMVector3TransformNormal(Xv, matRot);

	//	//加算
	//	pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
	//	pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
	//	pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;

	//	return pos;
	//}
	//XMFLOAT3 MoveRight(XMFLOAT3 pos)
	//{
	//	XMMATRIX matRot = XMMatrixIdentity();

	//	//X方向ベクトル
	//	Xv = { 0.5f,0.0f,0.0f,0.0f };

	//	//角度回転
	//	matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

	//	//X方向ベクトルを回転
	//	Xv = XMVector3TransformNormal(Xv, matRot);

	//	//加算
	//	pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
	//	pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
	//	pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;

	//	return pos;
	//}

	/// <summary>
	/// 進行方向に回転
	/// </summary>
	/// <param name="forward">進行方向ベクトル</param>
	/// <param name="upward">上ベクトル</param>
	/// <returns>回転行列（クォータニオン）</returns>
	XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward);

public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <param name="_model"></param>
	/// <returns></returns>
	static Player* Create(PMDmodel* _model);
	/// <summary>
	/// 初期化
	/// </summary>
	bool Initialize(PMDmodel* _model)override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(bool isShadow = false)override;

public:
	//void SetInput(const Input& input) { this->input = input; }
	void SetAction(int num) { this->Action = num; }
	void SetMoveVec(DirectX::XMFLOAT3 vec) { this->moveVec = vec; }
	void SetAvoidVec(DirectX::XMFLOAT3 vec) { this->avoidVec = vec; }

	int GetAction() { return Action; }
};