#include "PlayScene.h"
#include "Sprite/sprite.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Math/MyMath.h"
#include "Camera/DebugCamera.h"
#include "stage.h"
#include "object/Model.h"
#include <JsonLoader.h>

#include <future>

#include "Sprite/sprite.h"
#include <Singleton_Heap.h>

static const int debugTextTexNumber = 99;
std::thread th = {};

void PlayScene::Initialize(Wrapper* _dx12)
{
	assert(_dx12);

	this->dx12 = _dx12;

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

	//基本オブジェクト--------------
	defenceModel = Model::CreateFromOBJ("KSR-29");
	skyDomeModel = Model::CreateFromOBJ("skydome");
	bulletModel = Model::CreateFromOBJ("bullet");

	//ステージモデル
	stageModels.insert(std::make_pair("Ground", Model::CreateFromOBJ("Ground")));
	stageModels.insert(std::make_pair("Gate", Model::CreateFromOBJ("Gate")));
	stageModels.insert(std::make_pair("Wall", Model::CreateFromOBJ("Wall")));
	stageModels.insert(std::make_pair("Tree", Model::CreateFromOBJ("Tree")));
	stageModels.insert(std::make_pair("Cliff", Model::CreateFromOBJ("Cliff")));
	stageModels.insert(std::make_pair("Foundation", Model::CreateFromOBJ("Foundation")));
	stageModels.insert(std::make_pair("Spike", Model::CreateFromOBJ("spike")));
	stageModels.insert(std::make_pair("WallRock", Model::CreateFromOBJ("WallRock")));
	stageModels.insert(std::make_pair("Camp", Model::CreateFromOBJ("Camp")));
	stageModels.insert(std::make_pair("Tent", Model::CreateFromOBJ("Tent")));

	//カメラをセット
	mainCamera = new Camera(Application::window_width, Application::window_height);
	titleCamera = new Camera(Application::window_width, Application::window_height);
	setCamera = new Camera(Application::window_width, Application::window_height);

	titleCamera->SetEye(XMFLOAT3(0, 30.0f, -20.0f));
	titleCamera->SetTarget(XMFLOAT3(0, 30.0f, 0.0f));
	titleCamera->Update();

	Wrapper::SetCamera(titleCamera);
	FbxObject3d::SetCamera(dx12->Camera());
	Object3Ds::SetCamera(dx12->Camera());

	dx12->SceneUpdate();

	//光源
	light = Light::Create();
	light->SetDirLightActive(0, true);
	light->SetDirLightActive(1, true);
	light->SetDirLightActive(2, false);
	light->SetPointLightActive(0, true);
	light->SetPointLightAtten(0, XMFLOAT3(0.005f, 0.005f, 0.005f));
	light->SetPointLightActive(1, false);
	light->SetPointLightActive(2, false);
	light->SetCircleShadowActive(0, true);
	light->SetCircleShadowActive(1, true);
	light->SetCircleShadowActive(2, true);
	//ライトセット
	Wrapper::SetLight(light);

	/// <summary>
	/// ////////////////////////////////////////////////////////////
	/// </summary>
	/// <param name="_dx12"></param>
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

	//シーンエフェクト--------------------
//フェードイン・アウト
	fade.reset(Fade::Create(9, { 0.0f,0.0f }));
	fade.get()->SetAlpha(0.0f);
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

	int fontWidth = 64;
	int fontHeight = 64;

	float basePos = 680.0f;
	float offset = 50.f;

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
	Title = Sprite::Create(0, { 640.0f,120.0f });
	End = Sprite::Create(1, { 0.0f,0.0f });
	HpBer = Sprite::Create(3, { 0.0f,0.0f });
	Pose.reset(Sprite::Create(15, { 0.0f,0.0f }));

	Title->Update();

	///////////////////////////////////////////////////////////////////////////

	//ステージデータ及びモデルデータ読み込み
	stageData = JsonLoader::LoadJsonFile("titleStageData");
	for (auto& objectData : stageData->objects) {
		Model* model = nullptr;
		decltype(stageModels)::iterator it = stageModels.find(objectData.name);
		if (it != stageModels.end()) { model = it->second; }
		Stage* newObject = Stage::Create(model);

		//当たり判定をつけるかどうか
		if (objectData.name != "Tree" && objectData.name != "Cliff") {
			newObject->SetCollision();
		}
		//名前を付ける
		if (objectData.name == "Foundation") { newObject->SetObjectNum(ObjectType::FounDation); }
		if (objectData.name == "Gate") { newObject->SetObjectNum(ObjectType::Gate); }

		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		XMFLOAT3 rot{};
		XMStoreFloat3(&rot, objectData.rot);
		newObject->rotation = rot;

		XMFLOAT3 scale{};
		XMStoreFloat3(&scale, objectData.scale);
		newObject->scale = scale;

		titleStages.push_back(newObject);
	}

	//一度リセット（ステージの読み込みのため）
	stageData->objects = {};
	stageData = JsonLoader::LoadJsonFile("stageData");
	for (auto& objectData : stageData->objects) {
		Model* model = nullptr;
		decltype(stageModels)::iterator it = stageModels.find(objectData.name);
		if (it != stageModels.end()) { model = it->second; }

		Stage* newObject = Stage::Create(model);

		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		XMFLOAT3 rot{};
		XMStoreFloat3(&rot, objectData.rot);
		newObject->rotation = rot;

		XMFLOAT3 scale{};
		XMStoreFloat3(&scale, objectData.scale);
		newObject->scale = scale;

		if (objectData.name == "Foundation") {
			newObject->SetObjectNum(ObjectType::FounDation);
		}
		if (objectData.name == "Gate") {
			newObject->SetObjectNum(ObjectType::Gate);
		}

		stages.push_back(newObject);
	}

	//一度リセット（ベースキャンプの読み込みのため）
	stageData->objects = {};
	stageData = JsonLoader::LoadJsonFile("BaseCampData");
	for (auto& objectData : stageData->objects) {
		Model* model = nullptr;
		decltype(stageModels)::iterator it = stageModels.find(objectData.name);
		if (it != stageModels.end()) { model = it->second; }

		Stage* newObject = Stage::Create(model);

		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, objectData.trans);
		newObject->SetPosition(pos);

		XMFLOAT3 rot{};
		XMStoreFloat3(&rot, objectData.rot);
		newObject->rotation = rot;

		XMFLOAT3 scale{};
		XMStoreFloat3(&scale, objectData.scale);
		newObject->scale = scale;

		baseCamp.push_back(newObject);
	}
}

void PlayScene::Update()
{
	mainCamera->Update();
	titleCamera->Update();
	setCamera->Update();

	//ライト
	light->SetPointLightPos(0,
		XMFLOAT3(baseCamp[29]->position.x,
			baseCamp[29]->position.y + 5.0f,
			baseCamp[29]->position.z));

	//ステージ描画更新処理
	//タイトルステージ
	for (auto& object : titleStages) {
		object->SetColliderInvisible(false);
		object->Update();
	}
	for (auto& object : stages) {
		if (UseStage == GameLocation::BaseCamp) { object->SetColliderInvisible(false); }
		if (UseStage == GameLocation::BaseStage) {
			object->SetColliderInvisible(true);
			object->Update();
		}
	}
	for (auto& object : baseCamp) {
		if (UseStage == GameLocation::BaseStage) { object->SetColliderInvisible(false); }
		if (UseStage == GameLocation::BaseCamp) {
			object->SetColliderInvisible(true);
			object->Update();
		}
	}

	//タイトル進度0
	if (TitleWave == 0) {
		if (Input::GetInstance()->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			TitleWave++;
			titleStart = true;
			titleOption = false;
			keyFlag = true;
		}
	}
	//タイトル進度1
	else if (TitleWave == 1) {

		//項目選択（スタート、オプション）
		if (directInput->IsButtonPush(DirectInput::ButtonKind::DownButton) && TitleHierarchy < 1) { TitleHierarchy++; }
		if (directInput->IsButtonPush(DirectInput::ButtonKind::UpButton) && TitleHierarchy >= 1) { TitleHierarchy--; }

		//スタート選択状態
		if (TitleHierarchy == 0) {
			titleStart = true;
			titleOption = false;

			//タイトルからゲームへ
			if ((Input::GetInstance()->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) && !keyFlag) {
				fade.get()->SetFadeIn(true);
				start.get()->SetFadeIn(true);
			}
		}
		//オプション選択状態
		else if (TitleHierarchy == 1) {
			titleStart = false;
			titleOption = true;
		}
	}

	//フェードイン
	if (fade.get()->GetFadeIn()) {
		fade.get()->FadeIn();
		if (!fade.get()->GetFadeIn()) {
			Wrapper::SetCamera(mainCamera);
			fade.get()->SetFadeIn(false);
			fade.get()->SetFadeOut(true);

			load = true;
		}
	}

	if (load) {
		loading();

		if (!load) { SceneNum = Scene::GAME; }
	}

	keyFlag = false;

}

void PlayScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	if (!load) { Singleton_Heap::GetInstance()->FbxTexture = 200; }

	BaseObject::PreDraw(cmdList);
	for (auto& object : titleStages) {
		object->Draw();
	}
	BaseObject::PostDraw();

	// コマンドリストの取得
	Sprite::PreDraw(dx12->CommandList().Get());

	Title->Draw();

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

	//フェード用画像描画
	start.get()->Draw();
	if (fade.get()->GetFadeIn() || fade.get()->GetFadeOut() || fade.get()->GethalfFade()) {
		fade.get()->Draw();
	}
	if (clear.get()->GetClear()) {
		clear.get()->Draw();
	}
	else if (failed.get()->GetFailed()) {
		failed.get()->Draw();
	}

	//ローディング中
	if (load) {
		LoadControll.get()->Draw();
		for (int i = 0; i < 11; i++)
		{
			Now_Loading[i].get()->Draw();
		}
	}

	Sprite::PostDraw();
}

void PlayScene::ShadowDraw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	if (!load) { Singleton_Heap::GetInstance()->FbxTexture = 200; }

	//深度バッファクリア
	dx12->ClearDepthShadow();

	BaseObject::PreDraw(cmdList);

	for (auto& object : titleStages) {
		object->ShadowDraw();
	}
}

void PlayScene::asyncLoad()
{
	auto th = std::thread([&] {LoadResource(); });

	th.join();

	_loadMode = LoadMode::End;
}

void PlayScene::LoadResource()
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
	gateBreak_red.reset(Fade::Create(12, { 72,134 }));
	gateBreak_red.get()->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_red.get()->SetSize({ 80,80 });
	gateBreak_red.get()->Update();
	//門HP（黄状態）
	gateBreak_yellow.reset(Fade::Create(13, {72,134}));
	gateBreak_yellow.get()->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_yellow.get()->SetSize({ 80,80 });
	gateBreak_yellow.get()->Update();
	//門HP（）緑状態
	gateBreak_green.reset(Fade::Create(14, { 72,134 }));
	gateBreak_green.get()->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_green.get()->SetSize({ 80,80 });
	gateBreak_green.get()->Update();

	//スカイドーム-------------------
	skyDome = Object3Ds::Create(skyDomeModel);
	skyDome->scale = { 4,4,4 };
	skyDome->position = { 0,350,0 };

	//プレイヤー---------------------
	_player = Player::Create(FbxLoader::GetInstance()->LoadModelFromFile("testModel"));
	_player->CreateWeapon(Model::CreateFromOBJ("weapon"));
	_player->SetScale({ 0.2f, 0.2f, 0.2f });
	_player->SetPosition(XMFLOAT3(0.0f, 0.0f, 100.0f));
	_player->PlayAnimation();

	//エネミー-----------------------
	enemy = new EnemyManager();
	enemy = EnemyManager::Create(FbxLoader::GetInstance()->LoadModelFromFile("Golem"), FbxLoader::GetInstance()->LoadModelFromFile("Wolf"));

	//カメラの移動元の作成
	const float distanceFromPlayerToCamera = 10.0f;//カメラとプレイヤーの距離を決定
	const float cameraHeight = 25.0f;//カメラの高さ
	vv0 = { -1.0f,0.0f,0.0f,0.0f };//カメラの正面ベクトルを決定
	XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

	afterEye = _player->GetPosition();
	afterEye += direction * distanceFromPlayerToCamera;
	afterEye.y += cameraHeight;
	mainCamera->SetEye(afterEye);
	//カメラ注視点を決定
	XMFLOAT3 _target;
	_target = _player->GetPosition();
	_target.y += direction.y * distanceFromPlayerToCamera * 2.0f;
	mainCamera->SetTarget(_target);
}

void PlayScene::loading()
{
	if (load) {
		switch (_loadMode)
		{
		case LoadMode::No:
			_loadMode = LoadMode::Start;

			break;
			//何もない・・・
		case LoadMode::Start:
			//ローディング始め
			th = std::thread([&] {asyncLoad(); });
			_loadMode = LoadMode::Run;
			break;
		case LoadMode::Run:
			//ローディング中にやりたいこと

			//文字回転
			for (int i = 0; i < 11; i++)
			{
				static float angle = 0.0f;
				angle += 0.5f;
				Now_Loading[i].get()->SetRot(angle);
				Now_Loading[i].get()->Update();
			}

			break;
		case LoadMode::End:
			//ローディング終わり
			th.join();
			load = false;

		default:
			break;
		}
	}
}
