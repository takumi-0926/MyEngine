#include "PlayScene.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Math/MyMath.h"
#include "Camera/DebugCamera.h"
#include "stage.h"
#include "object/Model.h"
#include <JsonLoader.h>
#include "UIManager.h"

#include <future>

#include <Singleton_Heap.h>

static const int debugTextTexNumber = 99;
std::thread th = {};

void PlayScene::Initialize(Wrapper* _dx12)
{
	assert(_dx12);

	this->dx12 = _dx12;

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
	if (UIManager::GetInstance()->GetWave() == 0) {
		if (Input::GetInstance()->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			UIManager::GetInstance()->SetWave(1);
			UIManager::GetInstance()->SetStart(true);
			UIManager::GetInstance()->SetOption(false);

			keyFlag = true;
		}
	}
	//タイトル進度1
	else if (UIManager::GetInstance()->GetWave() == 1) {

		//項目選択（スタート、オプション）
		if (directInput->IsButtonPush(DirectInput::ButtonKind::DownButton) && UIManager::GetInstance()->GetHieraruchy() < 1) { UIManager::GetInstance()->SetHieraruchy(1); }
		if (directInput->IsButtonPush(DirectInput::ButtonKind::UpButton) && UIManager::GetInstance()->GetHieraruchy() >= 1) { UIManager::GetInstance()->SetHieraruchy(0); }

		//スタート選択状態
		if (UIManager::GetInstance()->GetHieraruchy() == 0) {
			UIManager::GetInstance()->SetStart(true);
			UIManager::GetInstance()->SetOption(false);

			//タイトルからゲームへ
			if ((Input::GetInstance()->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) && !keyFlag) {
				UIManager::GetInstance()->GetFade()->SetFadeIn(true);
				UIManager::GetInstance()->GetFade()->SetFadeIn(true);
			}
		}
		//オプション選択状態
		else if (UIManager::GetInstance()->GetHieraruchy() == 1) {
			UIManager::GetInstance()->SetStart(false);
			UIManager::GetInstance()->SetOption(true);
		}
	}

	//フェードイン
	if (UIManager::GetInstance()->GetFade()->GetFadeIn()) {
		UIManager::GetInstance()->GetFade()->FadeIn();
		if (!UIManager::GetInstance()->GetFade()->GetFadeIn()) {
			Wrapper::SetCamera(mainCamera);
			UIManager::GetInstance()->GetFade()->SetFadeIn(false);
			UIManager::GetInstance()->GetFade()->SetFadeOut(true);

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

	UIManager::GetInstance()->TitleDarw();

	if (load) {
		UIManager::GetInstance()->LoadDraw();
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

			UIManager::GetInstance()->LoadUpdate();

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
