#pragma once
#include <DirectXMath.h>
#include <map>
#include <vector>
#include "JsonLoader.h"

#include "UIManager.h"

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

const int ENEM_NUM = 1000;
const int P_HP = 100;

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

	bool pose = false;//�|�[�Y�t���O
	bool load = false;//���[�f�B���O
	int _loadMode = 0;//���[�f�B���O���
};

