#pragma once
#include "object\object3D.h"
#include "PMD\PMDmodel.h"
#include "DefCannon.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

#include "FBX/FbxObject3d.h"

#include "Weapon.h"
#include "ParticleManager.h"

enum Activity {
	golem = 1,
	wolf,
};
enum AttackType_Wolf {
	Type01_Walk,
	Type02_JumpWait,
	Type02_JumpAttack,
	Type03_ChargeAttack,
};
enum MotionType {
	WalkMotion = 0,
	AttackMotion_01,
	AttackMotion_02,
	AttackMotion_03,
};

class PMDmodel;
class StageObject;
class DefCannon;
class Enemy : public FbxObject3d {
	struct Status {//28
		int HP;
		float attack;	//攻撃力
		float defense;	//防御力
		float speed;
		XMFLOAT3 scale;	//大きさ
	};

	struct Jump {
		Vector3 pos;
		Vector3 p1;
		Vector3 p2;
		Vector3 p3;
	}jump;
public:
	//地面判定用
	bool OnGround = true;
	XMVECTOR fallV;

	Status status;//28
	bool alive = false;	//生存判定
	bool startAttack = false;
	bool attackHit = false;
	bool attackOnMove = false;
	bool damage = false;

	int actionPattern = 0;//行動パターン
	int attackPattern = 0;//攻撃パターン
	float attackTime = 0.0f;
	float jumpTime = 0.0f;
	float maxTime = 1.0f;
	int mode = 0;


	float damegeCount = 0.0f;
	//個体識別番号
	unsigned short myNumber = 0;
	int registrationNumber = 0;
	//パーティクル位置調整
	float particleOffset = 0.0f;
	//影位置調整
	float shadowOffset = 0.0f;
	//透明度
	float alpha = 1.0f;
	//デフォルト色
	vector<XMFLOAT3> defalt_ambient;
	XMVECTOR vectol;
	XMFLOAT3 attackPos;
	XMFLOAT3 oldPos;
	XMFLOAT3 RetreatPos = { 1.0f,20.0f,-150.0f };

	FbxModel* model = {};
	Weapon* weapon = nullptr;
	ParticleManager* particle = nullptr;

private:
	//Vector型をXMFLAT3型にキャスト
	XMFLOAT3 VectorToXMFloat(XMVECTOR vec);
	//二点間の距離を算出
	float objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2);	
	//二点間のベクトルを算出
	XMVECTOR objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2);	
	//正規化
	XMVECTOR Normalize(XMVECTOR vec);
	void Move(XMVECTOR vector);	//ベクトル移動
	void moveReset();
public:
	Enemy();		//コンストラクタ
	~Enemy();
	static unique_ptr<Enemy> Create(FbxModel* model, const int Type);//インスタンス生成
	void Initialize()override;
	void Update() override;
	void moveUpdate(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos);
	void Draw(ID3D12GraphicsCommandList* cmdList) override;

	void OnCollision(const CollisionInfo& info)override;

	/// <summary>
	/// 進行方向に回転
	/// </summary>
	/// <param name="forward">進行方向ベクトル</param>
	/// <param name="upward">上ベクトル</param>
	/// <returns>回転行列（クォータニオン）</returns>
	XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward) {
		Vector3 z = Vector3(forward.x, forward.y, forward.z);//進行方向ベクトル（前方向）
		Vector3 up = Vector3(upward.x, upward.y, upward.z);  //上方向
		XMMATRIX rot;//回転行列
		Quaternion q = quaternion(0, 0, 0, 1);//回転クォータニオン
		Vector3 _z = { 0.0f,0.0f,-1.0f };//Z方向単位ベクトル
		Vector3 cross;

		float a;//角度保存用
		float b;//角度保存用
		float c;//角度保存用
		float d;//角度保存用

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

		this->useRotMat = true;
		return rot;
	}

	void SetAlpha() {
		//for (int i = 0; i < model->GetMesh().size(); i++) {
		//	this->model->GetMesh()[i]->GetMaterial()->alpha = this->alpha;
		//	this->model->GetMesh()[i]->GetMaterial()->Update();
		//}
		model->alpha = this->alpha;
		model->Update();
	}

	void Particle();

	/// <summary>
	/// 出現処理
	/// </summary>
	void Appearance();

	/// <summary>
	/// 移動時処理
	/// </summary>
	void Move(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos);

	/// <summary>
	/// 攻撃時処理
	/// </summary>
	void Attack(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos);
	void JumpAttack(XMFLOAT3& targetPosition);
	void FingerAttack(XMFLOAT3& targetPosition);
	/// <summary>
	/// 退却時処理
	/// </summary>
	void Retreat();

	/// <summary>
	/// 被ダメージ時処理
	/// </summary>
	void Damage();
};