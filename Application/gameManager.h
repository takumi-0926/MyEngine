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

#include "..\stageObject.h"
#include "..\DefCannon.h"

#include "..\Vector3.h"
#include "..\Quaternion.h"

#include "..\SceneEffectManager.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

#define	ENEM_NUM	1000
#define P_HP		 100

enum Scene{
	TITLE,
	GAME,
	END
};
class Player;
class Stage;
class GameManager {
private://�����o�ϐ�(������)
	Input* input;
	Audio* audio;
	Wrapper* dx12;
	std::shared_ptr<PMDmodel> pmdModel;
	std::shared_ptr<PMDobject> pmdObject;
	PMDobject* player = nullptr;
	PMDobject* enemy = nullptr;
	PMDmodel* modelPlayer = nullptr;
private://�����o�ϐ�(�Q�[���V�[��)
	vector<Enemy*> _enemy;
	vector<Sqhere> sqhere;
	Stage* stage;
	Model* modelPlane = nullptr;
	Model* modelBox = nullptr;
	Model* modelPyramid = nullptr; 
	vector<Object3Ds*>stageObjects;
	StageObject* wall[6] = {};
	DefCannon* cannon[6] = {};
	Sprite* hp[P_HP] = {};
	Sprite* Damege = nullptr;
	HitBox* HitBox = {};

	Model* model01 = nullptr;
	Model* model02 = nullptr;
	Model* model03 = nullptr;
	Model* model04 = nullptr;
	Model* model05 = nullptr;
	Model* model06 = nullptr;
	Object3Ds* obj01 = nullptr;
	Object3Ds* obj02 = nullptr;
	Object3Ds* obj03 = nullptr;
	Object3Ds* obj04[10] = {};
	Sprite* sprite01 = nullptr;
	Sprite* sprite02 = nullptr;
	Sprite* sprite03 = nullptr;
	Sprite* sprite04 = nullptr;
	Sprite* sprite05 = nullptr;
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

	object2d* line[10] = {};

	Triangle triangle02[2] = {};
	Sqhere sqhere02 = {};

	Easing easing;

	int SceneNum = TITLE;

	int count = 24;

	bool resetFlag = false;

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

	float a;//�p�x�ۑ��p
	float b;//�p�x�ۑ��p
	float c;//�p�x�ۑ��p
	float d;//�p�x�ۑ��p

	bool isDamege = false;

	bool enemyToPlayerDamege = false;
	float DamegeAlpha = 1.0f;

public://�����o�֐�
	//�R���X�g���N�^
	GameManager();

	//�f�X�g���N�^
	~GameManager();

	//������
	bool Initalize(Wrapper* dx12, Audio* audio, Input* input);
	//�X�V
	void Update();
	//�`��
	void Draw();

	/// <summary>
	/// �ړ�
	/// </summary>
	/// <param name="pos">�ړ���������W</param>
	/// <returns>�ړ���̍��W</returns>
	XMFLOAT3 MoveBefore(XMFLOAT3 pos)
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
	XMFLOAT3 MoveAfter(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//Z�����x�N�g��
		Zv = { 0.0f,0.0f,0.5f,0.0f };

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
	XMFLOAT3 MoveLeft(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//X�����x�N�g��
		Xv = { 0.5f,0.0f,0.0f,0.0f };

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
	XMFLOAT3 MoveRight(XMFLOAT3 pos)
	{
		XMMATRIX matRot = XMMatrixIdentity();

		//X�����x�N�g��
		Xv = { 0.5f,0.0f,0.0f,0.0f };

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
	XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward) {
		Vector3 z = Vector3(forward.x, forward.y, forward.z);//�i�s�����x�N�g���i�O�����j
		Vector3 up = Vector3(upward.x, upward.y, upward.z);  //�����
		XMMATRIX rot;//��]�s��
		Quaternion q = quaternion(0,0,0,1);//��]�N�H�[�^�j�I��
		Vector3 _z = { 0.0f,0.0f,1.0f };//Z�����P�ʃx�N�g��
		Vector3 cross;
		XMMATRIX matRot = XMMatrixIdentity();

		//�J�����ɍ��킹�邽�߂̉�]�s��
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		cross = z.cross(_z);

		q.x = cross.x;
		q.y = cross.y;
		q.z = cross.z;

		q.w = sqrt(
			( z.length() *  z.length())
		   *(_z.length() * _z.length())) + z.dot(_z);

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
};
