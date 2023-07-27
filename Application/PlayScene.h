#pragma once
#include <DirectXMath.h>
#include "BsScene.h"
#include "UIManager.h"
#include "Collision/CollisionPrimitive.h"

class Wrapper;
class Camera;
class Light;
class Player;
class Object3Ds;
class Model;
class DefCannon;
class EnemyManager;
class ParticleManager;
class DebugCamera;

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
	PLAY,
	START,
	Preparation,
	POSE,
	WEAPONSELECT,
	SET,
	CLEAR,
	OVER,
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
class PlayScene : public BsScene
{
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(Wrapper* _dx12) override;

	void Finalize()override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;
	void ShadowDraw() override;

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
	int GameModeNum = GameMode::NONE;

	bool keyFlag = false;

private:
	Wrapper* dx12;		 //DirectX

	//�v���C���[ / �G�l�~�[
	Player* _player = nullptr;//�v���C���[�{��
	EnemyManager* enemy{};//�G�l�~�[�Ǘ�
	XMMATRIX rotM = {};//�x�N�g����]
	XMVECTOR vv0 = {};//�v���C���[�O�x�N�g��

	ParticleManager* particlemanager = nullptr;

	//���C�g
	Light* light = nullptr;
	XMFLOAT3 pointLightPos = {};

	//�J����
	Camera* mainCamera = nullptr;//�Q�[���J����
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
	Triangle triangle[50] = {};

	bool pose = false;//�|�[�Y�t���O
	bool load = false;//���[�f�B���O
	bool loadComplate = false;
	int _loadMode = 0;//���[�f�B���O���

	bool result = false;//�N���A����

	//////////////////////////////
		//�f�o�b�N�m�F�p�ϐ�
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowAtten[3] = { 0.5f,0.8f,0.0f };
	float circleShadowFacterAnlge[2] = { 0.1f,0.5f };
	float testPos[3] = { 1,0.0f,0 };
	int testNum[3] = { 0,0,0 };
	float debugCameraPos[3] = { 0,0,0 };
	float debugPointLightPos[3] = { 0,0,0 };
	float particleColor[4] = {};
	/////////////////////////////////////
};

