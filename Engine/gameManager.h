#pragma once
#include "dx12Wrapper.h"
#include "Input/input.h"
#include "Audio/audio.h"
#include "object/object3D.h"
#include "object/Model.h"
#include "Camera\DebugCamera.h"
#include "Collision\Collision.h"
#include "Sprite\sprite.h"
#include "PMD\pmdObject3D.h"
#include "PMD\PMDmodel.h"
#include "FBX\FbxObject3d.h"
#include "moving.h"
#include "light\Light.h"
#include "Easing.h"

#include "testObj.h"
#include "enemy.h"

#include "object\object2d.h"
#include "hitBox.h"
#include "shaderTest.h"

#include "DefCannon.h"

#include "Math/Vector3.h"
#include "Math/Quaternion.h"

#include "SceneEffect/SceneEffectManager.h"
#include "SceneEffect/Fade.h"

#include "Collision\MeshCollider.h"

#include "Sprite/DebugText.h"

#include "ParticleManager.h"
#include "SceneEffect/SceneEffect.h"
#include "BillboardObject.h"

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
enum GameMode {
	NASI,
	START,
	POSE,
	SET,
	CLEAR,
	OVER,
};
enum GameLocation {
	BaseCamp,
	BaseStage,
};

enum SpriteName {

};

enum modelName {

};

class Player;
class Stage;

struct JsonData;

class CollisionManager;
class GameManager {
private://�����o�ϐ�(������)
	Input*	 input;
	Audio*   audio;
	Wrapper*  dx12;
	DebugText debugText;

	std::shared_ptr<PMDmodel>   pmdModel;
	std::shared_ptr<PMDobject> pmdObject;
	Player*		   _player = nullptr;
	vector<Enemy*> _enemy;
	PMDmodel* modelPlayer = nullptr;

	Stage* stage;
	map<string, Model*> stageModels;
	vector<Stage*> stages;
	vector<Stage*> baseCamp;
	JsonData* stageData;
	JsonData* baseCampData;
	int UseStage = 0;

	Object3Ds* skyDome = nullptr;
	HitBox* HitBox = {};
	DefCannon* cannon[6] = {};
	Model* bulletModel = nullptr;
	FbxModel* golem[3] = {};
	FbxModel* wolf[3] = {};
	Model* skyDomeModel = nullptr;

	//�Փ˃}�l�[�W���[
	CollisionManager* collisionManager = nullptr;
	ParticleManager* particlemanager = nullptr;
	SceneEffect* sceneEffect = nullptr;
	BillboardObject* Bottom = nullptr;

	//���C�g
	Light* light = nullptr;
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowAtten[3] = { 0.5f,0.8f,0.0f };
	float circleShadowFacterAnlge[2] = { 0.0f,0.5f };
	float testPos[3] = { 1,0.0f,0 };

	Fade* fade = nullptr;//�V�[���؂�ւ���
	Fade* clear = nullptr;//�N���A��
	Fade* failed = nullptr;//�Q�[���I�[�o�[��
	Fade* start = nullptr;//�X�^�[�g��
	Fade* gateBreak = nullptr;
	bool result = false;


	DebugText* text = nullptr;
	Sprite* BreakBar = nullptr;
	Sprite* BreakGage[15] = {};
	Sprite* Pose = nullptr;
private://�����o�ϐ�(�Q�[���V�[��)
	vector<Sqhere> sqhere;
	Model* modelPlane = nullptr;
	Model* modelBox = nullptr;
	Model* modelPyramid = nullptr; 
	vector<Object3Ds*>stageObjects;
	Sprite* hp = nullptr;
	Sprite* Damege = nullptr;

	Model* model02 = nullptr;
	Model* model03 = nullptr;
	Model* model04 = nullptr;
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
	Camera* setCamera = nullptr;


	Plane plane[25] = {};
	Triangle triangle[50] = {};
	Ray ray;

	//object2d* line[10] = {};

	Triangle triangle02[2] = {};
	Sqhere sqhere02 = {};

	Easing easing;

	float particleColor[4] = {};

	//�V�[���ԍ�
	int SceneNum = TITLE;
	bool SceneChange = false;
	//�Q�[�����[�h
	int GameModeNum = GameMode::START;
	int count = 24;

	//�N���A�܂ł̕K�v������
	int ClearCount = 15;
	//���ސ�
	int repelCount = 0;

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

	bool isDamege = false;

	bool enemyToPlayerDamege = false;
	float DamegeAlpha = 1.0f;
	float popHp = 0;

	static const int debugTextTexNumber = 100;

	XMFLOAT3 afterEye;
	XMFLOAT3 setObjectPos;

	//�V�F�C�N�֌W
	float shakeTime = 20.0f;//�V�F�C�N����
	XMFLOAT3 BasePos;//�V�F�C�N�����W
	XMFLOAT3 shakeRand;//�V�F�C�N���Z�l
	bool PosDecision = false; //���W����
	bool shake = false;

	int SetNum = 0;

	bool pose = false;//�|�[�Y�t���O
public://�����o�֐�
	//�R���X�g���N�^
	GameManager();

	//�f�X�g���N�^
	~GameManager();

	//������
	bool Initalize(Wrapper* dx12, Audio* audio, Input* input);
	//�X�V
	void Update();

	void TitleUpdate();
	void GameUpdate();
	void EndUpdate();
	//�`��
	void Draw();

	void TitleDraw();
	void GameDraw();
	void EndDraw();

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

		float a;//�p�x�ۑ��p
		float b;//�p�x�ۑ��p
		float c;//�p�x�ۑ��p
		float d;//�p�x�ۑ��p

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

	/// <summary>
	/// �J�����̈ړ��i�w�肵���ꏊ�܂Łj
	/// </summary>
	/// <param name="pos1">���̈ʒu</param>
	/// <param name="pos2">�w��ꏊ</param>
	/// <param name="pct">�o�ߎ���</param>
	XMFLOAT3 moveCamera(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct);

	/// <summary>
	/// ����n�_����
	/// </summary>
	/// <param name="pos1"></param>
	/// <param name="pos2"></param>
	/// <returns></returns>
	bool samePoint(XMFLOAT3 pos1, XMFLOAT3 pos2) {
		if (pos1.x != pos2.x) { return false; }
		if (pos1.y != pos2.y) { return false; }
		if (pos1.z != pos2.z) { return false; }
		return true;
	}

	void Shake3D(XMFLOAT3& base) {
		//���Z�b�g
		if (shakeTime <= 1.0f) {
			shakeTime = 20.0f;
			base = BasePos;
			PosDecision = false;
			shake = false;
			return;
		}

		//�����W����
		if (!PosDecision) {
			BasePos = base;
			PosDecision = true;
		}

		//�V�F�C�N��������
		shakeRand.x = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);
		//shakeRand.y = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);
		shakeRand.z = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);

		//�V�F�C�N���Z
		base.x = BasePos.x + shakeRand.x;
		//base.y = BasePos.y + shakeRand.y;
		base.z = BasePos.z + shakeRand.z;

		//�V�F�C�N�^�C�����Z
		shakeTime -= 1.0f;

	}
};
