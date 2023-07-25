#include "UIManager.h"
#include "Sprite/sprite.h"
#include "SceneEffect/Fade.h"

void UIManager::Initialize()
{
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
	gateBreak_red = Fade::Create(12, { 72,134 });
	gateBreak_red->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_red->SetSize({ 80,80 });
	gateBreak_red->Update();
	//門HP（黄状態）
	gateBreak_yellow = Fade::Create(13, { 72,134 });
	gateBreak_yellow->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_yellow->SetSize({ 80,80 });
	gateBreak_yellow->Update();
	//門HP（）緑状態
	gateBreak_green = Fade::Create(14, { 72,134 });
	gateBreak_green->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_green->SetSize({ 80,80 });
	gateBreak_green->Update();
}

void UIManager::Update()
{
}

void UIManager::Darw()
{
}
