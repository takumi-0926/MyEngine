#pragma once
#include <DirectXMath.h>
#include <map>
#include <vector>
#include "JsonLoader.h"

class Wrapper;
class Camera;
class Light;
class Player;
class Stage;
class Object3Ds;
class Model;
class DefCannon;
class EnemyManager;
class ParticleManager;
class DebugCamera;

/// <summary>
/// /////////////////////////////////
/// </summary>

#include "SceneEffect/Fade.h"
class Sprite;

using namespace std;
using namespace DirectX;

enum Scene {
	TITLE,
	GAME,
	END,
	DebugTest,
	LOAD,
};

enum GameMode {
	NONE,
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

enum LoadMode {
	No,
	Start,
	Run,
	End,
};

enum SpriteName {
	Title_UI = 20,		//
	Title_UI_High,		//
	Title_UI_Low,		//
	Start_TItle_UI,		//
	Option_TItle_UI,	//
	Numbers,
	Start_UI_01,
	Start_UI_02,
	Start_UI_03,
};

const int ENEM_NUM = 1000;
const int P_HP = 100;
const int GATE_MAX = 10;

/// <summary>
/// �Q�[���v���C
/// </summary>
class PlayScene
{
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(Wrapper* _dx12);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();
	void ShadowDraw();

	void asyncLoad();

	void loading();

	void LoadResource();

private://���Z�b�g�Ώ�
	//�N���A�܂ł̕K�v������
	int ClearCount = 15;
	//���ސ�
	int repelCount = 0;
	//�V�[���ԍ�
	int SceneNum = TITLE;
	bool SceneChange = false;
	//�Q�[�����[�h
	int GameModeNum = GameMode::START;

	bool keyFlag = false;

private:
	Wrapper* dx12;		 //DirectX

	//�v���C���[ / �G�l�~�[
	Player* _player = nullptr;//�v���C���[�{��
	EnemyManager* enemy{};//�G�l�~�[�Ǘ�
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

	ParticleManager* particlemanager = nullptr;

	//���C�g
	Light* light = nullptr;
	XMFLOAT3 pointLightPos = {};

	//�J����
	Camera* mainCamera = nullptr;//�Q�[���J����
	Camera* titleCamera = nullptr;//�^�C�g���J����
	Camera* setCamera = nullptr;
	DebugCamera* camera = nullptr;
	XMFLOAT3 afterEye;
	XMFLOAT3 setObjectPos;

	//�h�q�{��
	DefCannon* defense_facilities[6] = {};//�S�h�q�{�ݏ��
	Model* defenceModel = nullptr;//�{�݃��f��
	Model* bulletModel = nullptr;//�e���f��
	float angleVertical, angleHorizonal;
	int SetNum = 0;

	/////////////////////////////////////////////////////////////////////////


	//�^�C�g������ϐ�
	unique_ptr<Sprite> TitleResources[2] = {};
	unique_ptr<Sprite> TitleResources_Start[3] = {};
	unique_ptr<Sprite> TitleResources_Option[3] = {};
	int TitleWave = 0;
	int TitleHierarchy = 0;
	bool titleStart = 0;
	bool titleOption = 0;

	//���UI
	unique_ptr<Sprite> screenUI[6];
	Sprite* weaponSelect = nullptr;
	Sprite* weaponSlot[3] = {};
	Sprite* Title = nullptr;
	Sprite* End = nullptr;
	Sprite* hp = nullptr;
	Sprite* HpBer = nullptr;
	bool result = false;//�N���A����

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


	//�e�N�X�`���G�t�F�N�g
	unique_ptr<Fade> Effect[8] = {};
	unique_ptr<Fade> fade = nullptr;//�V�[���؂�ւ���
	unique_ptr<Fade> clear = nullptr;//�N���A��
	unique_ptr<Fade> failed = nullptr;//�Q�[���I�[�o�[��
	unique_ptr<Fade> start = nullptr;//�X�^�[�g��
	unique_ptr<Fade> gateBreak_red = nullptr;//��ϋv�lUI�i�� : ��΂���j
	unique_ptr<Fade> gateBreak_yellow = nullptr;//��ϋv�lUI�i�� : �C��t���āj
	unique_ptr<Fade> gateBreak_green = nullptr;//��ϋv�lUI�i�� : ���v�j
	int gateHP = GATE_MAX;//��ϋv�l

	//�|�[�Y / ���[�h
	unique_ptr<Sprite> Pose = nullptr;
	unique_ptr<Sprite> Now_Loading[11] = {};
	unique_ptr<Sprite> LoadControll = nullptr;
	bool pose = false;//�|�[�Y�t���O
	bool load = false;//���[�f�B���O
	int _loadMode = 0;//���[�f�B���O���

	unique_ptr<Sprite> BreakCount[20] = {};
	unique_ptr<Sprite> BreakCountMax[20] = {};
	unique_ptr<Sprite> cross = nullptr;
};

