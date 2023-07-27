#pragma once
#include <DirectXMath.h>
#include "BsScene.h"
#include "UIManager.h"

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
/// ゲームプレイ
/// </summary>
class PlayScene : public BsScene
{
public:
	PlayScene(SceneManager* sceneManager_);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Wrapper* _dx12) override;

	void Finalize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
	void ShadowDraw() override;

	void asyncLoad();

	void loading();

	void LoadResource();

private://リセット対象
	//クリアまでの必要討伐数
	int ClearCount = 15;
	//撃退数
	int repelCount = 0;
	//シーン番号
	int SceneNum = TITLE;
	bool SceneChange = false;
	//ゲームモード
	int GameModeNum = GameMode::START;

	bool keyFlag = false;

private:
	Wrapper* dx12;		 //DirectX

	//プレイヤー / エネミー
	Player* _player = nullptr;//プレイヤー本体
	EnemyManager* enemy{};//エネミー管理
	XMMATRIX rotM = {};//ベクトル回転
	XMVECTOR vv0 = {};//プレイヤー前ベクトル

	ParticleManager* particlemanager = nullptr;

	//ライト
	Light* light = nullptr;
	XMFLOAT3 pointLightPos = {};

	//カメラ
	Camera* mainCamera = nullptr;//ゲームカメラ
	Camera* setCamera = nullptr;
	DebugCamera* camera = nullptr;
	XMFLOAT3 afterEye;
	XMFLOAT3 setObjectPos;

	//防衛施設
	DefCannon* defense_facilities[6] = {};//全防衛施設情報
	Model* defenceModel = nullptr;//施設モデル
	Model* bulletModel = nullptr;//弾モデル
	float angleVertical, angleHorizonal;
	int SetNum = 0;

	bool pose = false;//ポーズフラグ
	bool load = false;//ローディング
	int _loadMode = 0;//ローディング状態
};

