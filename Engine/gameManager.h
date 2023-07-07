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
#include "EnemyManager.h"

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
#define GATE_MAX      10

#define GUIDE_MODEL_NUM 1

enum Scene {
	TITLE,
	GAME,
	END,
	DebugTest,
	LOAD,
};
enum GameMode {
	NASI,
	START,
	Preparation,
	POSE,
	WEAPONSELECT,
	SET,
	CLEAR,
	OVER,
};
enum GameLocation {
	BaseCamp,
	BaseStage,
};

enum SpriteName {
	Title_UI = 19,		//
	Title_UI_High,		//
	Title_UI_Low,		//
	Start_TItle_UI,		//
	Option_TItle_UI,	//
	Numbers,
	Start_UI_01,
	Start_UI_02,
	Start_UI_03,
};

enum LoadMode {
	No,
	Start,
	Run,
	End,
};

enum modelName {

};

class Player;
class Stage;

struct JsonData;

class CollisionManager;
class GameManager {
private://�����o�ϐ�(������)
	Input* input;		 //�C���v�b�g
	Audio* audio;		 //�I�[�f�B�I
	Wrapper* dx12;		 //DirectX
	DebugText debugText; //�f�o�b�O�e�L�X�g

	//�J����
	Camera* mainCamera = nullptr;//�Q�[���J����
	Camera* titleCamera = nullptr;//�^�C�g���J����
	Camera* setCamera = nullptr;
	DebugCamera* camera = nullptr;
	XMFLOAT3 afterEye;
	XMFLOAT3 setObjectPos;

	//�v���C���[ / �G�l�~�[
	Player* _player = nullptr;//�v���C���[�{��
	PMDmodel* modelPlayer = nullptr;//�v���C���[���f��
	EnemyManager* enemy{};//�G�l�~�[�Ǘ�
	FbxModel* golem[3] = {};//�S�[�������f���iFBX�j
	FbxModel* wolf[3] = {};//�E���t���f���iFBX�j
	//HitBox* HitBox = {};//�q�b�g�{�b�N�X�i�v���C���[�p�j
	int useModel = 0;//�G�l�~�[���ʗp�ϐ�
	XMMATRIX rotM = {};//�x�N�g����]
	XMVECTOR vv0 = {};//�v���C���[�O�x�N�g��

	//�X�e�[�W
	int UseStage = 0;//�Q�[�����̃X�e�[�W���ʗp�ϐ�
	map<string, Model*> stageModels;//�X�e�[�W�Ŏg�p���郂�f���̊i�[��
	vector<Stage*>		titleStages;//�X�e�[�W���
	vector<Stage*>			 stages;//�X�e�[�W���
	vector<Stage*>		   baseCamp;//�x�[�X�L�����v���
	JsonData* stageData;//�X�e�[�W�\���ۑ��p
	Object3Ds* skyDome = nullptr;//�w�i�I�u�W�F�N�g
	Model* skyDomeModel = nullptr;//�w�i���f��

	//�h�q�{��
	DefCannon* defense_facilities[6] = {};//�S�h�q�{�ݏ��
	Model* defenceModel = nullptr;//�{�݃��f��
	Model* bulletModel = nullptr;//�e���f��
	float angleVertical, angleHorizonal;
	int SetNum = 0;

	//�Փ˃}�l�[�W���[
	CollisionManager* collisionManager = nullptr;
	ParticleManager* particlemanager = nullptr;
	SceneEffect* sceneEffect = nullptr;
	BillboardObject* Bottom = nullptr;

	//�R���W����
	vector<Sqhere> sqhere;
	Sqhere weaponCollider;
	Sqhere playerCollider;

	//���C�g
	Light* light = nullptr;
	XMFLOAT3 pointLightPos = {};

	//�e�N�X�`���G�t�F�N�g
	Fade* fade = nullptr;//�V�[���؂�ւ���
	Fade* clear = nullptr;//�N���A��
	Fade* failed = nullptr;//�Q�[���I�[�o�[��
	Fade* start = nullptr;//�X�^�[�g��
	Fade* gateBreak_red = nullptr;//��ϋv�lUI�i�� : ��΂���j
	Fade* gateBreak_yellow = nullptr;//��ϋv�lUI�i�� : �C��t���āj
	Fade* gateBreak_green = nullptr;//��ϋv�lUI�i�� : ���v�j
	int gateHP = GATE_MAX;//��ϋv�l

	//���UI
	Sprite* weaponSelect = nullptr;
	Sprite* weaponSlot[3] = {};
	Sprite* Title = nullptr;
	Sprite* End = nullptr;
	Sprite* hp = nullptr;
	Sprite* HpBer = nullptr;
	bool result = false;//�N���A����

	//�^�C�g������ϐ�
	unique_ptr<Sprite> TitleResources[2] = {};
	unique_ptr<Sprite> TitleResources_Start[3] = {};
	unique_ptr<Sprite> TitleResources_Option[3] = {};
	int TitleWave = 0;
	int TitleHierarchy = 0;
	bool titleStart = 0;
	bool titleOption = 0;

	unique_ptr<Sprite> BreakCount[20] = {};
	unique_ptr<Sprite> BreakCountMax[20] = {};
	unique_ptr<Sprite> cross = nullptr;
	//�N���A�܂ł̕K�v������
	int ClearCount = 15;
	//���ސ�
	int repelCount = 0;

	//�|�[�Y / ���[�h
	unique_ptr<Sprite> Pose = nullptr;
	unique_ptr<Sprite> Now_Loading[11] = {};
	bool pose = false;//�|�[�Y�t���O
	bool load = false;//���[�f�B���O
	int _loadMode = 0;//���[�f�B���O���

	//�J�E���gUI����ϐ�
	unique_ptr<Sprite> One_Numbers[10] = {};//0�`9�̐����X�v���C�g
	unique_ptr<Sprite> Ten_Numbers[10] = {};//0�`9�̐����X�v���C�g
	unique_ptr<Sprite> Start_UI_01 = {};
	unique_ptr<Sprite> Start_UI_02 = {};
	unique_ptr<Fade> Start_UI_03 = {};
	float calculationTime = 60.0f;//�v�Z�p
	int startTime = 0;//�J�n�J�E���g
	int one_place = 0;//��̈�
	int tens_place = 0;//�\�̈�

	int SlotCount = 0;
	int WeaponCount = 0;
	int UseFoundation = 0;
	bool WeaponSelectDo = false;

	bool keyFlag = false;
	//�Q�[�����K�C�h�I�u�W�F�N�g
	Object3Ds* moveGuide = nullptr;
	Model* guideModels[GUIDE_MODEL_NUM] = {};

	//�f�o�b�N�m�F�p�ϐ�
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowAtten[3] = { 0.5f,0.8f,0.0f };
	float circleShadowFacterAnlge[2] = { 0.1f,0.5f };
	float testPos[3] = { 1,0.0f,0 };
	int testNum[3] = { 0,0,0 };
	float debugCameraPos[3] = { 0,0,0 };
	float debugPointLightPos[3] = { 0,0,0 };
	float particleColor[4] = {};

	Object3Ds* debugFilde = nullptr;
	Model* debugFildeModel = nullptr;
	Object3Ds* debugCharacter = nullptr;
	Model* debugCharacterModel = nullptr;
	FbxModel* testModel = nullptr;
	FbxObject3d* testObject = nullptr;

	DebugText* text = nullptr;

private://�����o�ϐ�(�Q�[���V�[��)

	Plane plane[25] = {};
	Triangle triangle[50] = {};
	Ray ray;

	Easing easing;

	//�V�[���ԍ�
	int SceneNum = TITLE;
	bool SceneChange = false;
	//�Q�[�����[�h
	int GameModeNum = GameMode::START;
	int count = 24;

	bool resetFlag = false;

	XMMATRIX rot = {};
	float angle;
	bool colFlag;

	static const int debugTextTexNumber = 99;

	//�V�F�C�N�֌W�i���w�N���X�Ɉړ��j
	float shakeTime = 20.0f;//�V�F�C�N����
	XMFLOAT3 BasePos;//�V�F�C�N�����W
	XMFLOAT3 shakeRand;//�V�F�C�N���Z�l
	bool PosDecision = false; //���W����
	bool shake = false;

public://�����o�֐�
	//�R���X�g���N�^
	GameManager();

	//�f�X�g���N�^
	~GameManager();

	//������
	bool Initalize(Wrapper* dx12, Audio* audio, Input* input);
	void LoadTitleResources();//�^�C�g�����\�[�X�ǂݍ���
	void LoadGameResources();//�Q�[�������\�[�X�ǂݍ���
	void LoadAnotherResourecs();//���̑����\�[�X�ǂݍ���

	//�X�V
	void Update();

	void TitleUpdate();
	void GameUpdate();
	void EndUpdate();
	void DebugTestUpdate();

	void asyncLoad();

	void loading();

	void PlayerUpdate();
	void EnemyUpdate();

	//�`��
	void MainDraw();
	void SubDraw();
	void ShadowDraw(bool isShadow = false);

	void TitleDraw();
	void GameDraw();
	void EndDraw();

	void TitleReset();
	void GameReset();

	/// <summary>
	/// �i�s�����ɉ�]
	/// </summary>
	/// <param name="forward">�i�s�����x�N�g��</param>
	/// <param name="upward">��x�N�g��</param>
	/// <returns>��]�s��i�N�H�[�^�j�I���j</returns>
	inline XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward) {
		Vector3 z = Vector3(forward.x, forward.y, forward.z);//�i�s�����x�N�g���i�O�����j
		Vector3 up = Vector3(upward.x, upward.y, upward.z);  //�����
		XMMATRIX rot;//��]�s��
		Quaternion q = quaternion(0, 0, 0, 1);//��]�N�H�[�^�j�I��
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
			(z.length() * z.length())
			* (_z.length() * _z.length())) + z.dot(_z);

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
	inline bool samePoint(XMFLOAT3 pos1, XMFLOAT3 pos2) {
		if (pos1.x != pos2.x) { return false; }
		if (pos1.y != pos2.y) { return false; }
		if (pos1.z != pos2.z) { return false; }
		return true;
	}

	inline void Shake3D(XMFLOAT3& base) {
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

	inline float distance(XMFLOAT3 pos1, XMFLOAT3 pos2)
	{
		float distance;
		float x = abs(pos1.x - pos2.x);
		float z = abs(pos1.z - pos2.z);
		distance = std::sqrt(x * 2 + z * 2);
		return distance;
	}
};

XMFLOAT3 add(const XMFLOAT3& v1, const XMFLOAT3& v2);
