#pragma once
#include "dx12Wrapper.h"
#include "..\Input\input.h"
#include "..\Audio\audio.h"
#include "..\object\object3D.h"
#include "..\object\Model.h"
#include "..\Camera\DebugCamera.h"
#include "..\Collision\Collision.h"
#include "..\Sprite\sprite.h"
#include "..\PMD\pmdObject3D.h"
#include "..\PMD\PMDmodel.h"
#include "..\FBX\FbxObject3d.h"
#include "..\moving.h"
#include "..\light\Light.h"
#include "..\Easing.h"

#include "..\testObj.h"
#include "..\enemy.h"

#include "..\object\object2d.h"
#include "..\hitBox.h"
#include "..\shaderTest.h"

#include "..\stage.h"
#include "..\stageObject.h"
#include "..\DefCannon.h"

#include "..\Vector3.h"
#include "..\Quaternion.h"

#include "..\SceneEffectManager.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

#define	ENEM_NUM	1000
#define P_HP 100

enum {
	TITLE,
	GAME,
	END
};

class GameManager {
private://メンバ変数(初期化)
	Input* input;
	Audio* audio;
	Wrapper* dx12;
	std::shared_ptr<PMDmodel> pmdModel;
	std::shared_ptr<PMDobject> pmdObject;
private://メンバ変数(ゲームシーン)
	vector<Enemy*> _enemy;
	vector<Sqhere> sqhere;
	Stage* stage;
	StageObject* wall[6] = {};
	DefCannon* cannon[6] = {};
	Sprite* hp[P_HP] = {};

	Model* model01 = nullptr;
	Model* model02 = nullptr;
	Model* model03 = nullptr;
	Model* model04 = nullptr;
	Model* model05 = nullptr;
	Model* model06 = nullptr;
	Object3Ds* obj01 = nullptr;
	Object3Ds* obj02 = nullptr;
	Object3Ds* obj03 = nullptr;
	//Object3Ds* obj02[25] = {};
	Object3Ds* obj04[10] = {};
	Sprite* sprite01 = nullptr;
	Sprite* sprite02 = nullptr;
	Sprite* sprite03 = nullptr;
	Sprite* sprite04 = nullptr;
	Sprite* sprite05 = nullptr;
	Sprite* sprite06 = nullptr;
	PMDmodel* pModel = nullptr;
	PMDobject* pmdObj = nullptr;
	FbxModel* fbxModel1 = nullptr;
	FbxObject3d* fbxObj1 = nullptr;

	vector<Object3Ds> block;
	DebugCamera* camera = nullptr;
	Camera* mainCamera = nullptr;

	Light* light = nullptr;

	Plane plane[25] = {};
	Triangle triangle[50] = {};
	Ray ray;

	HitBox* HitBox[13] = {};
	object2d* line[10] = {};

	Triangle triangle02[2] = {};
	Sqhere sqhere02 = {};
	//運動
	Move move1{
		9.8f,	//重力
		0.0f,	//Y方向
		0.0f,	//X方向
		0.0f,	//初速度
		0.01f,	//加速度
		0.0f,	//空気抵抗
		0.0f,	//時間
		0.0f,	//速度
		1.0f,	//比例定数
		7.0f,	//質量
		move1.m * move1.v,	//運動量
		0.8f,

		false,	//移動開始フラグ
		0		//運動番号
	};
	Move move2{
	9.8f,	//重力
	0.0f,	//Y方向
	0.0f,	//X方向
	2.0f,	//初速度
	0.01f,	//加速度
	0.0f,	//空気抵抗
	0.0f,	//時間
	0.0f,	//速度
	1.0f,	//比例定数
	5.0f,	//質量
	move2.m * move2.v,	//運動量
	0.8f,

	false,	//移動開始フラグ
	0		//運動番号
	};

	Easing easing;

	int SceneNum = TITLE;

	int count = 24;

	bool resetFlag = false;

	bool shotFlag[10] = {};
	bool cubeFlag[25] = {};


	XMMATRIX rot = {};
	float angle;
	bool colFlag;

	XMMATRIX rotM = {};
	float angleVertical, angleHorizonal;
	XMVECTOR vv0 = {};

	XMVECTOR Zv;
	XMVECTOR Xv;
	XMVECTOR Yv;

	Obj* test[NUM_OBJ];
	int	_idx_obj = 0;

	int gateHP = 10;
	int playerHp = P_HP;
	int reception = 600;

	float speed = 0.0;

	float a;//角度保存用
	float b;//角度保存用
	float c;//角度保存用
	float d;//角度保存用

public://メンバ関数
	//コンストラクタ
	GameManager();

	//デストラクタ
	~GameManager();

	//初期化
	bool Initalize(Wrapper* dx12, Audio* audio, Input* input);

	//更新
	void Update();

	void titleUpdate();
	void gameUpdate();
	void endUpdate();

	//描画
	void Draw();

	void titleDraw();
	void gameDraw();
	void endDraw();

	XMFLOAT3 MoveBefore(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//Z方向ベクトル
		Zv = { 0.0f,0.0f,0.5f,0.0f };

		//弾角度回転
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//Z方向ベクトルを回転
		Zv = XMVector3TransformNormal(Zv, matRot);

		//加算
		pos.x += Zv.m128_f32[0] * directInput->getLeftY() * speed;
		pos.y += Zv.m128_f32[1] * directInput->getLeftY() * speed;
		pos.z += Zv.m128_f32[2] * directInput->getLeftY() * speed;
		//pos.x -= Zv.m128_f32[0];
		//pos.y -= Zv.m128_f32[1];
		//pos.z -= Zv.m128_f32[2];

		return pos;
	}
	XMFLOAT3 MoveAfter(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//Z方向ベクトル
		Zv = { 0.0f,0.0f,0.5f,0.0f };

		//弾角度回転
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//Z方向ベクトルを回転
		Zv = XMVector3TransformNormal(Zv, matRot);

		//加算
		pos.x += Zv.m128_f32[0] * directInput->getLeftY() * speed;
		pos.y += Zv.m128_f32[1] * directInput->getLeftY() * speed;
		pos.z += Zv.m128_f32[2] * directInput->getLeftY() * speed;
		//pos.x += Zv.m128_f32[0];
		//pos.y += Zv.m128_f32[1];
		//pos.z += Zv.m128_f32[2];

		return pos;
	}
	XMFLOAT3 MoveLeft(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//X方向ベクトル
		Xv = { 0.5f,0.0f,0.0f,0.0f };

		//角度回転
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//X方向ベクトルを回転
		Xv = XMVector3TransformNormal(Xv, matRot);

		//加算
		pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
		pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
		pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;
		//pos.x += Xv.m128_f32[0];
		//pos.y += Xv.m128_f32[1];
		//pos.z += Xv.m128_f32[2];

		return pos;
	}
	XMFLOAT3 MoveRight(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//X方向ベクトル
		Xv = { 0.5f,0.0f,0.0f,0.0f };

		//角度回転
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		//X方向ベクトルを回転
		Xv = XMVector3TransformNormal(Xv, matRot);

		//加算
		pos.x -= Xv.m128_f32[0] * directInput->getLeftX() * speed;
		pos.y -= Xv.m128_f32[1] * directInput->getLeftX() * speed;
		pos.z -= Xv.m128_f32[2] * directInput->getLeftX() * speed;
		//pos.x -= Xv.m128_f32[0];
		//pos.y -= Xv.m128_f32[1];
		//pos.z -= Xv.m128_f32[2];

		return pos;
	}

	XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward) {
		Vector3 z = Vector3(forward.x, forward.y, forward.z);//進行方向ベクトル（前方向）
		Vector3 up = Vector3(upward.x, upward.y, upward.z);  //上方向
		XMMATRIX rot;//回転行列
		Quaternion q = quaternion(0,0,0,1);//回転クォータニオン
		Vector3 _z = { 0.0f,0.0f,1.0f };//Z方向単位ベクトル
		Vector3 cross;
		XMMATRIX matRot = XMMatrixIdentity();

		//カメラに合わせるための回転行列
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		cross = z.cross(_z);

		q.x = cross.x;
		q.y = cross.y;
		q.z = cross.z;

		q.w = sqrt(
			( z.length() *  z.length())
		   *(_z.length() * _z.length())) + z.dot(_z);

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
};
