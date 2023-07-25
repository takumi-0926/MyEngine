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
/// ゲームプレイ
/// </summary>
class PlayScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Wrapper* _dx12);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	void ShadowDraw();

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

	//ステージ
	int UseStage = 0;//ゲーム中のステージ識別用変数
	map<string, Model*> stageModels;//ステージで使用するモデルの格納先
	vector<Stage*>		titleStages;//ステージ情報
	vector<Stage*>			 stages;//ステージ情報
	vector<Stage*>		   baseCamp;//ベースキャンプ情報
	JsonData* stageData;//ステージ構成保存用
	Object3Ds* skyDome = nullptr;//背景オブジェクト
	Model* skyDomeModel = nullptr;//背景モデル

	ParticleManager* particlemanager = nullptr;

	//ライト
	Light* light = nullptr;
	XMFLOAT3 pointLightPos = {};

	//カメラ
	Camera* mainCamera = nullptr;//ゲームカメラ
	Camera* titleCamera = nullptr;//タイトルカメラ
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

	/////////////////////////////////////////////////////////////////////////


	//タイトル周り変数
	unique_ptr<Sprite> TitleResources[2] = {};
	unique_ptr<Sprite> TitleResources_Start[3] = {};
	unique_ptr<Sprite> TitleResources_Option[3] = {};
	int TitleWave = 0;
	int TitleHierarchy = 0;
	bool titleStart = 0;
	bool titleOption = 0;

	//画面UI
	unique_ptr<Sprite> screenUI[6];
	Sprite* weaponSelect = nullptr;
	Sprite* weaponSlot[3] = {};
	Sprite* Title = nullptr;
	Sprite* End = nullptr;
	Sprite* hp = nullptr;
	Sprite* HpBer = nullptr;
	bool result = false;//クリア判定

	//カウントUI周り変数
	unique_ptr<Sprite> One_Numbers[10] = {};//0～9の数字スプライト
	unique_ptr<Sprite> Ten_Numbers[10] = {};//0～9の数字スプライト
	unique_ptr<Sprite> Start_UI_01 = {};
	unique_ptr<Sprite> Start_UI_02 = {};
	unique_ptr<Fade> Start_UI_03 = {};
	float calculationTime = 60.0f;//計算用
	int startTime = 0;//開始カウント
	int one_place = 0;//一の位
	int tens_place = 0;//十の位


	//テクスチャエフェクト
	unique_ptr<Fade> Effect[8] = {};
	unique_ptr<Fade> fade = nullptr;//シーン切り替え時
	unique_ptr<Fade> clear = nullptr;//クリア時
	unique_ptr<Fade> failed = nullptr;//ゲームオーバー時
	unique_ptr<Fade> start = nullptr;//スタート時
	unique_ptr<Fade> gateBreak_red = nullptr;//門耐久値UI（赤 : やばいよ）
	unique_ptr<Fade> gateBreak_yellow = nullptr;//門耐久値UI（黄 : 気を付けて）
	unique_ptr<Fade> gateBreak_green = nullptr;//門耐久値UI（緑 : 大丈夫）
	int gateHP = GATE_MAX;//門耐久値

	//ポーズ / ロード
	unique_ptr<Sprite> Pose = nullptr;
	unique_ptr<Sprite> Now_Loading[11] = {};
	unique_ptr<Sprite> LoadControll = nullptr;
	bool pose = false;//ポーズフラグ
	bool load = false;//ローディング
	int _loadMode = 0;//ローディング状態

	unique_ptr<Sprite> BreakCount[20] = {};
	unique_ptr<Sprite> BreakCountMax[20] = {};
	unique_ptr<Sprite> cross = nullptr;
};

