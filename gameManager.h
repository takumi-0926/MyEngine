#pragma once
#include "input.h"
#include "audio.h"
#include "dx12Wrapper.h"
#include "object3D.h"
#include "Model.h"
#include "DebugCamera.h"
#include "Collision.h"
#include "sprite.h"
#include "pmdObject3D.h"
#include "PMDmodel.h"
#include "moving.h"
#include "FbxObject3d.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

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
	Model* model01 = nullptr;
	Model* model02 = nullptr;
	Model* model03 = nullptr;
	Object3Ds* obj01 = nullptr;
	Object3Ds* obj02 = nullptr;
	//Object3Ds* obj02[25] = {};
	Object3Ds* obj03[10] = {};
	Sprite* sprite01 = nullptr;
	Sprite* sprite02 = nullptr;
	Sprite* sprite03 = nullptr;
	PMDmodel* pModel = nullptr;
	PMDobject* pmdObj = nullptr;
	FbxModel* fbxModel1 = nullptr;
	FbxObject3d* fbxObj1 = nullptr;

	vector<Object3Ds> block;
	DebugCamera* camera = nullptr;

	Plane plane[25] = {};
	Triangle triangle[50] = {};
	Sqhere sqhere[10] = {};
	Ray ray;

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

	int SceneNum = TITLE;

	int count = 24;

	bool resetFlag = false;

	bool shotFlag[10] = {};
	bool cubeFlag[25] = {};


	XMMATRIX rot = {};
	float angle;
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
};