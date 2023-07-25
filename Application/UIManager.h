#pragma once
#include <memory>
#include "SceneEffect/Fade.h"

using namespace std;

const int GATE_MAX = 10;

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

class Sprite;
/// <summary>
/// UI管理クラス（シングルトン）
/// </summary>
class UIManager
{
public:
	UIManager() = default;
	~UIManager() {};

	static UIManager* GetInstance();

private:
	UIManager(const UIManager& manager) = delete;
	UIManager& operator=(const UIManager& heap) = delete;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void TitleUpdate();
	void GameUpdate();
	void EndUpdate();
	void LoadUpdate();
	/// <summary>
	/// 描画
	/// </summary>
	void TitleDarw();
	void GameDarw();
	void ENdDarw();
	void LoadDraw();

private://識別変数関係変数
	int TitleWave = 0;
	int TitleHierarchy = 0;
	bool titleStart = 0;
	bool titleOption = 0;

public://識別変数関係ゲッター
	int GetWave() { return TitleWave; }
	int GetHieraruchy() { return TitleHierarchy; }
	bool GetStart() { return titleStart; }
	bool GetOption() { return titleOption; }

	void SetWave(int wave) { this->TitleWave = wave; }
	void SetHieraruchy(int hierarchy) { this->TitleHierarchy = hierarchy; }
	void SetStart(bool flag) { this->titleStart = flag; }
	void SetOption(bool flag) { this->titleOption = flag; }

private:
	//画面UI
	unique_ptr<Sprite> screenUI[6];
	Sprite* weaponSelect = nullptr;
	Sprite* weaponSlot[3] = {};
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

	//タイトル周り変数
	unique_ptr<Sprite> TitleResources[2] = {};
	unique_ptr<Sprite> TitleResources_Start[3] = {};
	unique_ptr<Sprite> TitleResources_Option[3] = {};
	unique_ptr<Sprite> Title = nullptr;

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

	unique_ptr<Sprite> BreakCount[20] = {};
	unique_ptr<Sprite> BreakCountMax[20] = {};
	unique_ptr<Sprite> cross = nullptr;

public:
	Fade* GetFade() { return fade.get(); }
};

