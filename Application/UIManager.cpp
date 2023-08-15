#include "UIManager.h"
#include "Sprite/sprite.h"
#include "SceneEffect/Fade.h"

#include "Input/input.h"

static const int debugTextTexNumber = 99;

UIManager* UIManager::GetInstance()
{
	static UIManager* instance = new UIManager();
	return instance;
}

void UIManager::Initialize()
{
	//this->audio->Load();// デバッグテキスト用テクスチャ読み込み
	if (!Sprite::loadTexture(debugTextTexNumber, L"Resources/debugfont.png")) { assert(0); }

	//画像リソース
	if (!Sprite::loadTexture(0, L"Resources/Title.dds")) { assert(0); }
	if (!Sprite::loadTexture(1, L"Resources/end.png")) { assert(0); }
	if (!Sprite::loadTexture(2, L"Resources/haikei.png")) { assert(0); }
	if (!Sprite::loadTexture(3, L"Resources/HpBer.png")) { assert(0); }
	if (!Sprite::loadTexture(4, L"Resources/Hp.png")) { assert(0); }
	if (!Sprite::loadTexture(6, L"Resources/start.dds")) { assert(0); }
	if (!Sprite::loadTexture(7, L"Resources/clear_result.dds")) { assert(0); }
	if (!Sprite::loadTexture(8, L"Resources/failed_result.png")) { assert(0); }
	if (!Sprite::loadTexture(9, L"Resources/blackTex.dds")) { assert(0); }
	if (!Sprite::loadTexture(10, L"Resources/breakBer.png")) { assert(0); }
	if (!Sprite::loadTexture(11, L"Resources/breakGage.png")) { assert(0); }
	if (!Sprite::loadTexture(12, L"Resources/GateUI_red.png")) { assert(0); }
	if (!Sprite::loadTexture(13, L"Resources/GateUI_yellow.png")) { assert(0); }
	if (!Sprite::loadTexture(14, L"Resources/GateUI.png")) { assert(0); }
	if (!Sprite::loadTexture(15, L"Resources/pose.png")) { assert(0); }
	if (!Sprite::loadTexture(16, L"Resources/weapon.png")) { assert(0); }
	if (!Sprite::loadTexture(17, L"Resources/weaponSlot.png")) { assert(0); }
	if (!Sprite::loadTexture(18, L"Resources/loading.dds")) { assert(0); }
	if (!Sprite::loadTexture(19, L"Resources/controll.png")) { assert(0); }

	if (!Sprite::loadTexture(SpriteName::Title_UI, L"Resources/Title_UI_01.png")) { assert(0); }
	if (!Sprite::loadTexture(SpriteName::Title_UI_High, L"Resources/Title_UI_high.png")) { assert(0); }
	if (!Sprite::loadTexture(SpriteName::Title_UI_Low, L"Resources/Title_UI_low.png")) { assert(0); }
	if (!Sprite::loadTexture(SpriteName::Start_TItle_UI, L"Resources/Title_UI_start.png")) { assert(0); }
	if (!Sprite::loadTexture(SpriteName::Option_TItle_UI, L"Resources/Title_UI_option.png")) { assert(0); }

	TitleResources[0].reset(Sprite::Create(Title_UI_High, { 1280.0f - 440.0f,512.0f }));
	TitleResources[1].reset(Sprite::Create(Title_UI, { 1280.0f - 440.0f,512.0f }));
	TitleResources_Start[0].reset(Sprite::Create(Title_UI_High, { 1280.0f - 440.0f,512.0f }));
	TitleResources_Start[1].reset(Sprite::Create(Title_UI_Low, { 1280.0f - 440.0f,512.0f }));
	TitleResources_Start[2].reset(Sprite::Create(Start_TItle_UI, { 1280.0f - 440.0f,512.0f }));
	TitleResources_Option[0].reset(Sprite::Create(Title_UI_High, { 1280.0f - 440.0f,556.0f }));
	TitleResources_Option[1].reset(Sprite::Create(Title_UI_Low, { 1280.0f - 440.0f,556.0f }));
	TitleResources_Option[2].reset(Sprite::Create(Option_TItle_UI, { 1280.0f - 440.0f,556.0f }));

	TitleResources[0].get()->SetSize(XMFLOAT2(440.0f, 44.0f));
	TitleResources[1].get()->SetSize(XMFLOAT2(440.0f, 44.0f));
	for (int i = 0; i < 3; i++) {
		TitleResources_Start[i].get()->SetSize(XMFLOAT2(440.0f, 44.0f));
		TitleResources_Option[i].get()->SetSize(XMFLOAT2(440.0f, 44.0f));
	}

	for (int i = 0; i < 2; i++) { TitleResources[i].get()->Update(); }
	for (int i = 0; i < 3; i++) { TitleResources_Start[i].get()->Update(); }
	for (int i = 0; i < 3; i++) { TitleResources_Option[i].get()->Update(); }

	if (!Sprite::loadTexture(SpriteName::Numbers, L"Resources/Numbers.dds")) { assert(0); }
	if (!Sprite::loadTexture(SpriteName::Start_UI_01, L"Resources/Start_UI_01.png")) { assert(0); }
	if (!Sprite::loadTexture(SpriteName::Start_UI_02, L"Resources/Start_UI_02.png")) { assert(0); }
	if (!Sprite::loadTexture(SpriteName::Start_UI_03, L"Resources/Start_UI_03.png")) { assert(0); }

	int fontWidth = 32;
	int fontHeight = 48;

	float basePos = 32.0f;
	float offset = 3.5f;

	for (int i = 0; i < 10; i++)
	{
		//生成
		One_Numbers[i].reset(Sprite::Create(SpriteName::Numbers, { 640.0f - offset ,basePos }));
		Ten_Numbers[i].reset(Sprite::Create(SpriteName::Numbers, { 640.0f - float(fontWidth) + offset, basePos }));
		//指定の数字を枠切り
		One_Numbers[i].get()->SetTextureRect({ float(fontWidth * i), 0 }, { float(fontWidth), float(fontHeight) });
		Ten_Numbers[i].get()->SetTextureRect({ float(fontWidth * i), 0 }, { float(fontWidth), float(fontHeight) });
		//サイズ設定
		One_Numbers[i].get()->SetSize({ float(fontWidth),float(fontHeight) });
		Ten_Numbers[i].get()->SetSize({ float(fontWidth),float(fontHeight) });
		//更新
		One_Numbers[i].get()->Update();
		Ten_Numbers[i].get()->Update();
	}

	cross.reset(Sprite::Create(SpriteName::Numbers, { 698.f,320.f }));
	cross.get()->SetTextureRect({ 0, float(fontHeight) }, { float(fontWidth), float(fontHeight) });
	cross.get()->SetSize({ float(fontWidth),float(fontHeight) });
	cross.get()->Update();
	for (int i = 0; i < 20; i++) {
		BreakCount[i].reset(Sprite::Create(SpriteName::Numbers, { 0,0 }));
		BreakCountMax[i].reset(Sprite::Create(SpriteName::Numbers, { 0,0 }));
		BreakCount[i].get()->SetTextureRect({ float(fontWidth * i), 0 }, { float(fontWidth), float(fontHeight) });
		BreakCountMax[i].get()->SetTextureRect({ float(fontWidth * i), 0 }, { float(fontWidth), float(fontHeight) });
		BreakCount[i].get()->SetSize({ float(fontWidth),float(fontHeight) });
		BreakCountMax[i].get()->SetSize({ float(fontWidth),float(fontHeight) });
		BreakCount[i].get()->Update();
		BreakCountMax[i].get()->Update();
	}

	Start_UI_01.reset(Sprite::Create(SpriteName::Start_UI_01, { 640.0f,basePos + 84.0f }));
	Start_UI_01.get()->SetAnchorPoint({ 0.5f,0.5f });
	Start_UI_01.get()->Update();

	Start_UI_02.reset(Sprite::Create(SpriteName::Start_UI_02, { 640.0f,basePos + 24.0f }));
	Start_UI_02.get()->SetAnchorPoint({ 0.5f,0.5f });
	Start_UI_02.get()->Update();

	Start_UI_03.reset(Fade::Create(SpriteName::Start_UI_03, { 640.0f,360.0f }));
	Start_UI_03.get()->SetAnchorPoint({ 0.5f,0.5f });
	Start_UI_03.get()->Update();

	//門HP（赤状態）
	gateBreak_red.reset(Fade::Create(12, { 72,134 }));
	gateBreak_red.get()->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_red.get()->SetSize({ 80,80 });
	gateBreak_red.get()->Update();
	//門HP（黄状態）
	gateBreak_yellow.reset(Fade::Create(13, { 72,134 }));
	gateBreak_yellow.get()->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_yellow.get()->SetSize({ 80,80 });
	gateBreak_yellow.get()->Update();
	//門HP（）緑状態
	gateBreak_green.reset(Fade::Create(14, { 72,134 }));
	gateBreak_green.get()->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_green.get()->SetSize({ 80,80 });
	gateBreak_green.get()->Update();

	//シーンエフェクト--------------------
//フェードイン・アウト
	fade.reset(Fade::Create(9, { 0.0f,0.0f }));
	fade.get()->SetAlpha(0.0f);
	fade.get()->Update();

	//クリア時UI
	clear.reset(Fade::Create(7, { Application::window_width / 2,Application::window_height / 2 }));
	clear.get()->SetAnchorPoint({ 0.5f,0.5f });
	clear.get()->SetSize({ 480,480 });
	//ゲームオーバー時UI
	failed.reset(Fade::Create(8, { Application::window_width / 2,Application::window_height / 2 }));
	failed.get()->SetAnchorPoint({ 0.5f,0.5f });
	failed.get()->SetSize({ 480,480 });
	//スタート時UI
	start.reset(Fade::Create(6, { 0,80 }));
	start.get()->SetAnchorPoint({ 0.5f,0.5f });
	start.get()->SetAlpha(0.0f);
	start.get()->SetSize({ 360,360 });
	start.get()->Update();

	fontWidth = 64;
	fontHeight = 64;

	basePos = 680.0f;
	offset = 50.f;

	for (int i = 0; i < 11; i++)
	{
		Now_Loading[i].reset(Sprite::Create(18, { 740.0f + offset * i ,basePos }));
		Now_Loading[i].get()->SetTextureRect({ float(fontWidth * i), 0 }, { float(fontWidth), float(fontHeight) });
		Now_Loading[i].get()->SetSize({ float(fontWidth),float(fontHeight) });
		Now_Loading[i].get()->SetAnchorPoint({ 0.5f,0.5f });
		Now_Loading[i].get()->Update();
	}
	LoadControll.reset(Sprite::Create(19, { 640.0f,360.0f }));
	LoadControll.get()->SetAnchorPoint({ 0.5f,0.5f });
	LoadControll.get()->Update();

	//スプライト---------------------
	Title.reset(Sprite::Create(0, { 640.0f,120.0f }));
	End = Sprite::Create(1, { 0.0f,0.0f });
	HpBer = Sprite::Create(3, { 0.0f,0.0f });
	Pose.reset(Sprite::Create(15, { 0.0f,0.0f }));

	Title.get()->Update();

	hp = Sprite::Create(4, { 36.0f,32.0f });
	hp->Update();
}

void UIManager::TitleUpdate()
{
}

void UIManager::GameUpdate()
{
	if (mode == 2) {
		//スタート時画像表示
		start->SlideIn();
		start->FadeIn();
		start->SlideOut();
		if (start->GetSlideOut()) {
			start->FadeOut();
			if (!start->GetFadeOut()) {
				Start_UI_03.get()->SetFadeIn(true);
			}
		}
		start->Update();
	}

	////カウント更新
	//startTime = int(calculationTime);
	//one_place = startTime % 10;
	//tens_place = startTime / 10;
	//calculationTime -= 1.0f / 50.0f;

	if (Input::GetInstance()->Trigger(DIK_SPACE)) {
		gateBreak_red->SetShake(true);
		gateBreak_yellow->SetShake(true);
		gateBreak_green->SetShake(true);
	}

	gateBreak_red->Update();
	gateBreak_yellow->Update();
	gateBreak_green->Update();

	hp->Update();
}

void UIManager::EndUpdate()
{
}

void UIManager::LoadUpdate()
{
	//文字回転
	for (int i = 0; i < 11; i++)
	{
		static float angle = 0.0f;
		angle += 0.5f;
		Now_Loading[i].get()->SetRot(angle);
		Now_Loading[i].get()->Update();
	}
}

void UIManager::TitleDarw()
{
	Title.get()->Draw();

	if (TitleWave == 0) {
		for (int i = 0; i < 2; i++) { TitleResources[i].get()->Draw(); }
	}
	else if (TitleWave == 1) {
		if (titleStart) { TitleResources_Start[0].get()->Draw(); }
		else { TitleResources_Start[1].get()->Draw(); }
		TitleResources_Start[2].get()->Draw();

		if (titleOption) { TitleResources_Option[0].get()->Draw(); }
		else { TitleResources_Option[1].get()->Draw(); }
		TitleResources_Option[2].get()->Draw();
	}
}

void UIManager::GameDarw()
{
	if ((GATE_MAX - 3) <= gateHP) {
		gateBreak_green->Draw();
	}
	else if ((GATE_MAX - 6) <= gateHP) {
		gateBreak_yellow->Draw();
	}
	else if ((GATE_MAX - 9) <= gateHP) {
		gateBreak_red->Draw();
	}

	hp->Draw();
	HpBer->Draw();

	//修正
	static float pos = 12.f;
	static float pos_h = 240.f;
	static float offset = 24.f;
	cross.get()->SetPos({ pos + offset * 2 ,pos_h });
	cross.get()->Update();
	cross.get()->Draw();

	BreakCountMax[1].get()->SetPos({ pos + offset * 3,pos_h });
	BreakCountMax[15].get()->SetPos({ pos + offset * 4,pos_h });
	BreakCountMax[1].get()->Update();
	BreakCountMax[15].get()->Update();
	BreakCountMax[1].get()->Draw();
	BreakCountMax[15].get()->Draw();

	if (repelCount >= 10) {
		BreakCount[repelCount - 10].get()->SetPos({ pos + offset,pos_h });
		BreakCount[11].get()->SetPos({ pos,pos_h });
		BreakCount[repelCount - 10].get()->Update();
		BreakCount[11].get()->Update();
		BreakCount[11].get()->Update();
		BreakCount[repelCount - 10].get()->Draw();
		BreakCount[11].get()->Draw();
	}
	else {
		BreakCount[repelCount].get()->SetPos({ pos + offset * 0.5f,pos_h });
		BreakCount[repelCount].get()->Update();
		BreakCount[repelCount].get()->Draw();
	}
}

void UIManager::ENdDarw()
{
}

void UIManager::LoadDraw()
{
	LoadControll.get()->Draw();
	for (int i = 0; i < 11; i++)
	{
		Now_Loading[i].get()->Draw();
	}
}

void UIManager::FadeDraw()
{
	//フェード用画像描画
	start.get()->Draw();
	fade.get()->Draw();
	if (clear.get()->GetClear()) {
		clear.get()->Draw();
	}
	else if (failed.get()->GetFailed()) {
		failed.get()->Draw();
	}
}

void UIManager::PoseDraw()
{
	Pose->Draw();
}

void UIManager::StartDraw()
{
	////カウント周り
	//Start_UI_02.get()->Draw();
	//Start_UI_01.get()->Draw();
	//Start_UI_03.get()->Draw();
	//One_Numbers[one_place].get()->Draw();
	//Ten_Numbers[tens_place].get()->Draw();
}
