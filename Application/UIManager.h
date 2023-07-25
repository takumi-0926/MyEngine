#pragma once
#include <memory>

using namespace std;

const int GATE_MAX = 10;

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

class Fade;
class Sprite;
/// <summary>
/// UI管理クラス（シングルトン）
/// </summary>
class UIManager
{
public:
	UIManager() = default;
	~UIManager() {};

private:
	UIManager(const UIManager& manager) = delete;
	UIManager& operator=(const UIManager& heap) = delete;

public:
	void Initialize();
	void Update();
	void Darw();

private:
	//テクスチャエフェクト
	unique_ptr<Fade> Effect[8];
	Fade* fade = nullptr;//シーン切り替え時
	Fade* clear = nullptr;//クリア時
	Fade* failed = nullptr;//ゲームオーバー時
	Fade* start = nullptr;//スタート時
	Fade* gateBreak_red = nullptr;//門耐久値UI（赤 : やばいよ）
	Fade* gateBreak_yellow = nullptr;//門耐久値UI（黄 : 気を付けて）
	Fade* gateBreak_green = nullptr;//門耐久値UI（緑 : 大丈夫）

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

	unique_ptr<Sprite> BreakCount[20] = {};
	unique_ptr<Sprite> BreakCountMax[20] = {};
	unique_ptr<Sprite> cross = nullptr;

};

