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
	FbxModel* fbxModel1 = nullptr;
	FbxObject3d* fbxObj1 = nullptr;

	vector<Object3Ds> block;
	DebugCamera* camera = nullptr;

	Plane plane[25] = {};
	Triangle triangle[50] = {};
	Sqhere sqhere[10] = {};
	Ray ray;

	//�^��
	Move move{
		9.8f,	//�d��
		0.0f,	//Y����
		0.0f,	//X����
		2.0f,	//�����x
		1.0f,	//�����x
		0.5f,	//��C��R
		0.0f,	//����
		0.0f	//���x
	};

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

	void titleUpdate();
	void gameUpdate();
	void endUpdate();

	//�`��
	void Draw();

	void titleDraw();
	void gameDraw();
	void endDraw();
};