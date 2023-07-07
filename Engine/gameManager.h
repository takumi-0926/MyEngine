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
private://メンバ変数(初期化)
	Input* input;		 //インプット
	Audio* audio;		 //オーディオ
	Wrapper* dx12;		 //DirectX
	DebugText debugText; //デバッグテキスト

	//カメラ
	Camera* mainCamera = nullptr;//ゲームカメラ
	Camera* titleCamera = nullptr;//タイトルカメラ
	Camera* setCamera = nullptr;
	DebugCamera* camera = nullptr;
	XMFLOAT3 afterEye;
	XMFLOAT3 setObjectPos;

	//プレイヤー / エネミー
	Player* _player = nullptr;//プレイヤー本体
	PMDmodel* modelPlayer = nullptr;//プレイヤーモデル
	EnemyManager* enemy{};//エネミー管理
	FbxModel* golem[3] = {};//ゴーレムモデル（FBX）
	FbxModel* wolf[3] = {};//ウルフモデル（FBX）
	//HitBox* HitBox = {};//ヒットボックス（プレイヤー用）
	int useModel = 0;//エネミー識別用変数
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

	//防衛施設
	DefCannon* defense_facilities[6] = {};//全防衛施設情報
	Model* defenceModel = nullptr;//施設モデル
	Model* bulletModel = nullptr;//弾モデル
	float angleVertical, angleHorizonal;
	int SetNum = 0;

	//衝突マネージャー
	CollisionManager* collisionManager = nullptr;
	ParticleManager* particlemanager = nullptr;
	SceneEffect* sceneEffect = nullptr;
	BillboardObject* Bottom = nullptr;

	//コリジョン
	vector<Sqhere> sqhere;
	Sqhere weaponCollider;
	Sqhere playerCollider;

	//ライト
	Light* light = nullptr;
	XMFLOAT3 pointLightPos = {};

	//テクスチャエフェクト
	Fade* fade = nullptr;//シーン切り替え時
	Fade* clear = nullptr;//クリア時
	Fade* failed = nullptr;//ゲームオーバー時
	Fade* start = nullptr;//スタート時
	Fade* gateBreak_red = nullptr;//門耐久値UI（赤 : やばいよ）
	Fade* gateBreak_yellow = nullptr;//門耐久値UI（黄 : 気を付けて）
	Fade* gateBreak_green = nullptr;//門耐久値UI（緑 : 大丈夫）
	int gateHP = GATE_MAX;//門耐久値

	//画面UI
	Sprite* weaponSelect = nullptr;
	Sprite* weaponSlot[3] = {};
	Sprite* Title = nullptr;
	Sprite* End = nullptr;
	Sprite* hp = nullptr;
	Sprite* HpBer = nullptr;
	bool result = false;//クリア判定

	//タイトル周り変数
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
	//クリアまでの必要討伐数
	int ClearCount = 15;
	//撃退数
	int repelCount = 0;

	//ポーズ / ロード
	unique_ptr<Sprite> Pose = nullptr;
	unique_ptr<Sprite> Now_Loading[11] = {};
	bool pose = false;//ポーズフラグ
	bool load = false;//ローディング
	int _loadMode = 0;//ローディング状態

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

	int SlotCount = 0;
	int WeaponCount = 0;
	int UseFoundation = 0;
	bool WeaponSelectDo = false;

	bool keyFlag = false;
	//ゲーム内ガイドオブジェクト
	Object3Ds* moveGuide = nullptr;
	Model* guideModels[GUIDE_MODEL_NUM] = {};

	//デバック確認用変数
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

private://メンバ変数(ゲームシーン)

	Plane plane[25] = {};
	Triangle triangle[50] = {};
	Ray ray;

	Easing easing;

	//シーン番号
	int SceneNum = TITLE;
	bool SceneChange = false;
	//ゲームモード
	int GameModeNum = GameMode::START;
	int count = 24;

	bool resetFlag = false;

	XMMATRIX rot = {};
	float angle;
	bool colFlag;

	static const int debugTextTexNumber = 99;

	//シェイク関係（数学クラスに移動）
	float shakeTime = 20.0f;//シェイク時間
	XMFLOAT3 BasePos;//シェイク元座標
	XMFLOAT3 shakeRand;//シェイク加算値
	bool PosDecision = false; //座標決定
	bool shake = false;

public://メンバ関数
	//コンストラクタ
	GameManager();

	//デストラクタ
	~GameManager();

	//初期化
	bool Initalize(Wrapper* dx12, Audio* audio, Input* input);
	void LoadTitleResources();//タイトルリソース読み込み
	void LoadGameResources();//ゲーム内リソース読み込み
	void LoadAnotherResourecs();//その他リソース読み込み

	//更新
	void Update();

	void TitleUpdate();
	void GameUpdate();
	void EndUpdate();
	void DebugTestUpdate();

	void asyncLoad();

	void loading();

	void PlayerUpdate();
	void EnemyUpdate();

	//描画
	void MainDraw();
	void SubDraw();
	void ShadowDraw(bool isShadow = false);

	void TitleDraw();
	void GameDraw();
	void EndDraw();

	void TitleReset();
	void GameReset();

	/// <summary>
	/// 進行方向に回転
	/// </summary>
	/// <param name="forward">進行方向ベクトル</param>
	/// <param name="upward">上ベクトル</param>
	/// <returns>回転行列（クォータニオン）</returns>
	inline XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward) {
		Vector3 z = Vector3(forward.x, forward.y, forward.z);//進行方向ベクトル（前方向）
		Vector3 up = Vector3(upward.x, upward.y, upward.z);  //上方向
		XMMATRIX rot;//回転行列
		Quaternion q = quaternion(0, 0, 0, 1);//回転クォータニオン
		Vector3 _z = { 0.0f,0.0f,1.0f };//Z方向単位ベクトル
		Vector3 cross;
		XMMATRIX matRot = XMMatrixIdentity();

		float a;//角度保存用
		float b;//角度保存用
		float c;//角度保存用
		float d;//角度保存用

		//カメラに合わせるための回転行列
		matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

		cross = z.cross(_z);

		q.x = cross.x;
		q.y = cross.y;
		q.z = cross.z;

		q.w = sqrt(
			(z.length() * z.length())
			* (_z.length() * _z.length())) + z.dot(_z);

		//単位クォータニオン化
		q = normalize(q);
		q = conjugate(q);
		a = q.x;
		b = q.y;
		c = q.z;
		d = q.w;

		//任意軸回転
		XMVECTOR rq = { q.x,q.y,q.z,q.w };
		rot = XMMatrixRotationQuaternion(rq);

		return rot;
	}

	/// <summary>
	/// カメラの移動（指定した場所まで）
	/// </summary>
	/// <param name="pos1">元の位置</param>
	/// <param name="pos2">指定場所</param>
	/// <param name="pct">経過時間</param>
	XMFLOAT3 moveCamera(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct);

	/// <summary>
	/// 同一地点判別
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
		//リセット
		if (shakeTime <= 1.0f) {
			shakeTime = 20.0f;
			base = BasePos;
			PosDecision = false;
			shake = false;
			return;
		}

		//元座標決定
		if (!PosDecision) {
			BasePos = base;
			PosDecision = true;
		}

		//シェイク乱数決定
		shakeRand.x = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);
		//shakeRand.y = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);
		shakeRand.z = rand() % int(shakeTime) - (int(shakeTime) / 3.0f);

		//シェイク加算
		base.x = BasePos.x + shakeRand.x;
		//base.y = BasePos.y + shakeRand.y;
		base.z = BasePos.z + shakeRand.z;

		//シェイクタイム減算
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
