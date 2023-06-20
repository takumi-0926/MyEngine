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
	Attack2,
	Attack3,
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
	/// ステータス
	/// </summary>
	struct Status {
		float HP;
		float Attack;
	};

	DirectX::XMFLOAT3 moveVec = {};
	XMVECTOR Zv, Xv, Yv;
	XMVECTOR _v;

	Weapon* weapon = nullptr;
	Sqhere collision = {};

	bool move{}; //移動中true
	bool attack{}; //攻撃中true
	bool avoid{}; //回避中true

	//回避用変数
	DirectX::XMFLOAT3 avoidVec = {};
	float avoidSpeed = 1.8f;
	float avoidTime = 0.0f;

	//攻撃用変数
	int combo = 0;
	int attackNum = action::Attack;
	float freamCount = 0;
	bool atCombo = false;

	int Action = 0;

	float speed = 0.0;
	float angleVertical, angleHorizonal;

	//地面判定用
	bool OnGround = true;
	XMVECTOR fallV;

	int followBoneNum = 0;

	bool Hit = false;
	bool Damage = false;

private:
	void actionExecution(int num);
	void moveUpdate();

	void Attack();
	void Avoid(XMFLOAT3& vec);

	/// <summary>
/// 移動
/// </summary>
/// <param name="pos">移動させる座標</param>
/// <returns>移動後の座標</returns>
	inline XMFLOAT3 MoveBefore(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//Z方向ベクトル
		Zv = { 0.0f,0.0f,1.0f,0.0f };

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
	inline XMFLOAT3 MoveAfter(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//Z方向ベクトル
		Zv = { 0.0f,0.0f,1.0f,0.0f };

		//弾角度回転
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//Z方向ベクトルを回転
		Zv = XMVector3TransformNormal(Zv, matRot);

		//加算
		pos.x += Zv.m128_f32[0] * directInput->getLeftY() * speed;
		pos.y += Zv.m128_f32[1] * directInput->getLeftY() * speed;
		pos.z += Zv.m128_f32[2] * directInput->getLeftY() * speed;

		return pos;
	}
	inline XMFLOAT3 MoveLeft(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//X方向ベクトル
		Xv = { 1.0f,0.0f,0.0f,0.0f };

		//角度回転
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//X方向ベクトルを回転
		Xv = XMVector3TransformNormal(Xv, matRot);

		//加算
		pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
		pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
		pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;

		return pos;
	}
	inline XMFLOAT3 MoveRight(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//X方向ベクトル
		Xv = { 1.0f,0.0f,0.0f,0.0f };

		//角度回転
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//X方向ベクトルを回転
		Xv = XMVector3TransformNormal(Xv, matRot);

		//加算
		pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
		pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
		pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;

		return pos;
	}

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
	static Player* Create(FbxModel* model);
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList)override;

	void CreateWeapon(Model* model);

public:
	//void SetInput(const Input& input) { this->input = input; }
	inline void SetAction(int num) { this->Action = num; }
	inline void SetMoveVec(DirectX::XMFLOAT3 vec) { this->moveVec = vec; }
	inline void SetAvoidVec(DirectX::XMFLOAT3 vec) { this->avoidVec = vec; }
	inline void SetHit(bool flag) { this->Hit = flag; }
	inline void SetDamage(bool flag) { this->Damage = flag; }

	inline void SetAngleH(float angle) { this->angleHorizonal = angle;}

	int GetAction() { return Action; }

	inline XMFLOAT3 GetPos() { return position; }
	inline float GetAngleVertical() { return angleVertical; }
	inline float GetAngleHorizonal() { return angleHorizonal; }
	inline bool GetHit() { return Hit; }
	inline bool GetDamage() { return Damage; }
	inline bool GetAttack() { return attack; }
	inline Sqhere GetCollision() { return collision; }
	inline Weapon* GetInstance() {
		Weapon* instance = new Weapon();
		return instance;
	}
};