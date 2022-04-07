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

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

enum {
	TITLE,
	GAME,
	END
};

class GameManager {
private://�����o�ϐ�(������)
	Input* input;
	Audio* audio;
	Wrapper* dx12;

private://�����o�ϐ�(�Q�[���V�[��)
	Model* model01 = nullptr;
	Model* model02 = nullptr;
	Model* model03 = nullptr;
	Object3Ds* obj01 = nullptr;
	Object3Ds* obj02[25] = {};
	Object3Ds* obj03[10] = {};
	Sprite* sprite01 = nullptr;
	Sprite* sprite02 = nullptr;
	Sprite* sprite03 = nullptr;
	PMDmodel* pModel = nullptr;
	PMDobject* pmdObj = nullptr;

	vector<Object3Ds> block;
	DebugCamera* camera = nullptr;

	Plane plane[25] = {};
	Triangle triangle[50] = {};
	Sqhere sqhere[10] = {};
	Ray ray;

	int SceneNum = TITLE;

	int count = 24;

	bool resetFlag = false;

	bool shotFlag[10] = {};
	bool cubeFlag[25] = {};

public://�����o�֐�
	//�R���X�g���N�^
	GameManager();

	//�f�X�g���N�^
	~GameManager();

	//������
	bool Initalize(Wrapper* dx12,Audio* audio,Input* input);

	//�X�V
	void Update();

	//�`��
	void Draw();
};