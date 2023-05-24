#include <algorithm>
#include <future>
#include "gameManager.h"
#include "object\baseObject.h"
#include "FBX\FbxLoader.h"
#include "FBX\FbxObject3d.h"
#include "PMD/PMXLoader.h"
#include "stage.h"
#include "Player.h"
#include "JsonLoader.h"

#include"DebugImgui.h"

#include "Collision\MeshCollider.h"
#include "Collision\SphereCollider.h"
#include "Collision\CollisionManager.h"

std::thread th = {};

GameManager::GameManager()
{
}
GameManager::~GameManager()
{

}

bool GameManager::Initalize(Wrapper* dx12, Audio* audio, Input* input)
{
	// nullptrチェック
	assert(dx12);
	assert(input);
	assert(audio);

	this->dx12 = dx12;
	this->input = input;
	this->audio = audio;

	//this->audio->Load();// デバッグテキスト用テクスチャ読み込み
	if (!Sprite::loadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
		return false;
	}
	debugText.Initialize(debugTextTexNumber);

	//画像リソース
	if (!Sprite::loadTexture(0, L"Resources/Title.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(1, L"Resources/end.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(2, L"Resources/haikei.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(3, L"Resources/HpBer.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(4, L"Resources/Hp.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(6, L"Resources/start.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(7, L"Resources/clear_result.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(8, L"Resources/failed_result.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(9, L"Resources/blackTex.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(10, L"Resources/breakBer.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(11, L"Resources/breakGage.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(12, L"Resources/GateUI_red.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(13, L"Resources/GateUI_yellow.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(14, L"Resources/GateUI.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(15, L"Resources/pose.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(16, L"Resources/weapon.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(17, L"Resources/weaponSlot.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(18, L"Resources/loading.png")) {
		assert(0);
		return false;
	}

	if (!BillboardObject::LoadTexture(0, L"Resources/GateUI_red.png")) {
		assert(0);
		return false;
	}

	LoadTitleResources();

	//カメラをセット
	camera = new DebugCamera(Application::window_width, Application::window_height, input);
	mainCamera = new Camera(Application::window_width, Application::window_height);
	titleCamera = new Camera(Application::window_width, Application::window_height);
	setCamera = new Camera(Application::window_width, Application::window_height);

	camera->SetEye(XMFLOAT3(50.0f, 100.0f, 0));
	camera->Update();

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

	debugFildeModel = Model::CreateFromOBJ("Ground");
	debugFilde = Object3Ds::Create(debugFildeModel);
	debugCharacterModel = Model::CreateFromOBJ("Box");
	debugCharacter = Object3Ds::Create(debugCharacterModel);

	particlemanager = ParticleManager::Create();
	particlemanager->Update();

	Bottom = BillboardObject::Create(0);
	Bottom->Update();

	Bottom->CreateObject(XMFLOAT3(0, 10, 0), 5);

	testModel = FbxLoader::GetInstance()->LoadModelFromFile("testModel");
	testObject = new FbxObject3d;
	testObject->Initialize();
	testObject->SetModel(testModel);
	testObject->PlayAnimation();

	//基本オブジェクト--------------
	defenceModel = Model::CreateFromOBJ("KSR-29");
	skyDomeModel = Model::CreateFromOBJ("skydome");
	bulletModel = Model::CreateFromOBJ("bullet");
	Box1x1 = Model::CreateFromOBJ("Box");

	for (int i = 0; i < 3; i++) {
		golem[i] = FbxLoader::GetInstance()->LoadModelFromFile("Golem");
		wolf[i] = FbxLoader::GetInstance()->LoadModelFromFile("Wolf");
	}

	//ステージデータ及びモデルデータ読み込み
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

	//MMDオブジェクト----------------
	//modelPlayer = PMDmodel::CreateFromPMD("Resources/Model/初音ミク.pmd");
	//modelPlayer->LoadVMDFile(vmdData::WAIT, "Resources/vmd/marieru_stand.vmd");
	//modelPlayer->LoadVMDFile(vmdData::WALK, "Resources/vmd/Rick式走りモーション02.vmd");
	//modelPlayer->LoadVMDFile(vmdData::ATTACK, "Resources/vmd/attack.vmd");
	//modelPlayer->LoadVMDFile(vmdData::DAMAGE, "Resources/vmd/腹部ダメージモーション.vmd");
	//modelPlayer->LoadVMDFile(vmdData::AVOID, "Resources/vmd/Rick式走りモーション05.vmd");

	//スプライト---------------------
	Title = Sprite::Create(0, { 640.0f,120.0f });
	End = Sprite::Create(1, { 0.0f,0.0f });
	HpBer = Sprite::Create(3, { 0.0f,0.0f });
	Pose = Sprite::Create(15, { 0.0f,0.0f });

	Title->Update();

	weaponSelect = Sprite::Create(16, { 0.0f,0.0f });
	for (int i = 0; i < 3; i++)
	{
		weaponSlot[i] = Sprite::Create(17, { 455.0f + (176.0f * i),551.0f });
		weaponSlot[i]->SetAnchorPoint({ 0.5f,0.5f });
	}
	hp = Sprite::Create(4, { 36.0f,32.0f });
	hp->SetSize(XMFLOAT2(playerHp * 4.5f, 30));
	hp->Update();

	BreakBar = Sprite::Create(10, { 540,640 });
	BreakBar->Update();
	for (int i = 0; i < 15; i++) {
		static float xpos = 48.0f;
		BreakGage[i] = Sprite::Create(11, { xpos * i + 540.0f,640 });
		BreakGage[i]->Update();
	}

	//シーンエフェクト--------------------
	//フェードイン・アウト
	fade = Fade::Create(9, { 0.0f,0.0f });
	fade->SetAlpha(0.0f);
	//クリア時UI
	clear = Fade::Create(7, { Application::window_width / 2,Application::window_height / 2 });
	clear->SetAnchorPoint({ 0.5f,0.5f });
	clear->SetSize({ 480,480 });
	//ゲームオーバー時UI
	failed = Fade::Create(8, { Application::window_width / 2,Application::window_height / 2 });
	failed->SetAnchorPoint({ 0.5f,0.5f });
	failed->SetSize({ 480,480 });
	//スタート時UI
	start = Fade::Create(6, { 0,80 });
	start->SetAnchorPoint({ 0.5f,0.5f });
	start->SetAlpha(0.0f);
	start->SetSize({ 360,360 });
	start->Update();

	loadResource.reset(Sprite::Create(18, { 640.0f, 360.0f }));
	loadResource.get()->Update();

	//入力及び音声
	input->Update();

	SceneNum = TITLE;

	particlemanager->CreateParticle(30, XMFLOAT3(0, 0, 0), 0.01f, 0.01f, 12, 4.0f, { 0.2f,0.2f,0.8f,1 }, 1);

	return true;
}
void GameManager::Update()
{
	//imgui
	static bool blnChk = false;
	static int radio = 0;
	static float eneSpeed = 0.0f;
	{
		ImGui::Begin("Rendering Test Menu");
		ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);
		//imguiのUIコントロール
		//ImGui::Text("PlayerPosition : %.2f %.2f", _player->GetPosition().x, _player->GetPosition().z);
		//ImGui::Text("ClearResultPos : %.2f %.2f", clear->Pos().x, clear->Pos().y);
		//ImGui::Text("ClearResultPos : %.2f %.2f", protEnemy[0]->weapon->position.x, protEnemy[0]->weapon->position.z);
		//ImGui::Text("ClearResultPos : %.2f %.2f", protEnemy[0]->GetPosition().x, protEnemy[0]->GetPosition().z);
		ImGui::Checkbox("EnemyPop", &blnChk);
		ImGui::RadioButton("Game Mode", &radio, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Debug Mode", &radio, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Set Camera", &radio, 2);
		int nSlider = 0;
		ImGui::SliderFloat("Enemy Speed", &eneSpeed, 0.05f, 1.0f);
		static float col3[3] = {};
		ImGui::ColorPicker3("ColorPicker3", col3, ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
		static float col4[4] = {};
		ImGui::ColorPicker4("ColorPicker4", col4, ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

		ImGui::InputFloat3("circleShadowDir", circleShadowDir);
		ImGui::InputFloat3("circleShadowAtten", circleShadowAtten);
		ImGui::InputFloat2("circleShadowFactorAngle", circleShadowFacterAnlge);
		ImGui::InputFloat3("DebugCameraEye", debugCameraPos);
		ImGui::InputFloat3("DebugWeaponPos", testPos);
		ImGui::InputFloat3("Pos", debugPointLightPos);
		ImGui::InputInt3("DebugBoneNum", testNum);

		ImGui::End();

		ImGui::Begin("Particle");
		ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);
		ImGui::ColorPicker4("ColorPicker4", particleColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
		ImGui::End();

		DebugImgui::UpdateImgui();

		//カメラ切り替え
		static bool isCamera = false;
		//if (radio == 0 && isCamera == false) {
		//	Wrapper::SetCamera(mainCamera);
		//	FbxObject3d::SetCamera(dx12->Camera());
		//	Object3Ds::SetCamera(dx12->Camera());

		//	isCamera = true;
		//}
		//else if (radio == 1 && isCamera == true) {
		//	Wrapper::SetCamera(camera);
		//	FbxObject3d::SetCamera(dx12->Camera());
		//	Object3Ds::SetCamera(dx12->Camera());

		//	SceneNum = Scene::DebugTest;

		//	isCamera = false;
		//}
	}

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

	camera->Update();
	mainCamera->Update();
	titleCamera->Update();
	setCamera->Update();

	TitleUpdate();
	GameUpdate();
	EndUpdate();
	DebugTestUpdate();
}

void GameManager::TitleUpdate()
{
	//タイトル更新
	if (SceneNum != TITLE)return;

	//if(directInput->IsButtonPush(directInput->DownButton))

	//タイトル進度0
	if (TitleWave == 0) {
		if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			TitleWave++;
			titleStart = true;
			titleOption = false;
			keyFlag = true;
			//Title->Update();
			//fade->SetFadeIn(true);
			//start->SetFadeIn(true);
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
			if ((input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) && !keyFlag) {
				fade->SetFadeIn(true);
				start->SetFadeIn(true);
			}
		}
		//オプション選択状態
		else if (TitleHierarchy == 1) {
			titleStart = false;
			titleOption = true;
		}
	}

	//フェードイン
	if (fade->GetFadeIn()) {
		fade->FadeIn();
		if (!fade->GetFadeIn()) {
			Wrapper::SetCamera(mainCamera);
			fade->SetFadeIn(false);
			fade->SetFadeOut(true);

			load = true;
			TitleReset();
		}
	}

	if (load) {
		loading();

		if (!load) { SceneNum = Scene::GAME; }
	}

	keyFlag = false;
}
void GameManager::GameUpdate() {
	//ゲーム
	if (SceneNum == GAME) {

		//フェードアウト
		if (fade->GetFadeOut() && !load) {
			fade->FadeOut();
		}

		//当たり判定確認
		//CollisionManager::GetInstance()->CheckAllCollision();

		//if (GameModeNum == GameMode::LOAD) {
		//	loading();
		//	if (!load) {
		//		GameModeNum = GameMode::START;
		//	}
		//	return;
		//}

		//ゲームスタート時処理
		if (GameModeNum == GameMode::START) {

			_player->SetPosition(XMFLOAT3(0.0f, 0.0f, 100.0f));
			_player->Update();

			//カメラの移動先の作成
			static float step = 0;
			const float distanceFromPlayerToCamera = 50.0f;//カメラとプレイヤーの距離を決定
			const float cameraHeight = 30.0f;//カメラの高さ
			vv0 = { 0.0f,0.0f,1.0f,0.0f };//カメラの正面ベクトルを決定
			XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

			XMFLOAT3 beforeEye;
			beforeEye.x = _player->GetPosition().x + direction.x * distanceFromPlayerToCamera;
			beforeEye.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera;
			beforeEye.z = _player->GetPosition().z + direction.z * distanceFromPlayerToCamera;
			beforeEye.y += cameraHeight * 1.3f;

			//カメラ注視点を決定
			XMFLOAT3 _target;
			_target.x = _player->GetPosition().x;
			_target.y = _player->GetPosition().y;
			_target.z = _player->GetPosition().z;
			_target.y += cameraHeight;
			mainCamera->SetTarget(_target);
			mainCamera->SetEye(moveCamera(mainCamera->GetEye(), beforeEye, step += 0.0005f));
			XMFLOAT3 e = mainCamera->GetEye();
			if (samePoint(e, beforeEye)) {
				GameModeNum = GameMode::NASI;
				step = 0.0f;
			}

			//スタート時画像表示
			start->SlideIn();
			start->FadeIn();
			start->SlideOut();
			if (start->GetSlideOut()) {
				start->FadeOut();
				if (!start->GetFadeOut()) {
					Start_UI_03.get()->SetFadeIn(true);
					GameModeNum = GameMode::NASI;
				}
			}
			start->Update();
		}
		//ゲーム進行前処理
		else if (GameModeNum == GameMode::Preparation) {
			PlayerUpdate();

			Start_UI_03.get()->UpIn();
			if (Start_UI_03.get()->GetDownOut()) {
				Start_UI_03.get()->DownOut();
			}

			//カウント更新
			startTime = int(calculationTime);
			one_place = startTime % 10;
			tens_place = startTime / 10;
			calculationTime -= 1.0f / 50.0f;

			//スペース長押しでゲームスタート
			static int count = 0;
			if (input->Push(DIK_SPACE)) {
				count++;
				if (count >= 120) { GameModeNum = GameMode::NASI; }
			}
		}
		//ゲーム進行中処理
		else if (GameModeNum == GameMode::NASI) {
			if (UseStage != GameLocation::BaseCamp) {
				EnemyUpdate();
			}
			PlayerUpdate();

			//当たり判定（プレイヤー / 敵 / 最終関門）
			{
				for (int i = 0; i < sqhere.size(); i++) {
					bool Hhit = Collision::CheckSqhere2Sqhere(sqhere[i], playerCollider);
					XMVECTOR inter;
					bool Ghit = Collision::CheckSqhere2Triangle(sqhere[i], triangle[0], &inter);

					//ゲート攻撃
					if (Ghit == true && reception <= 0 && _enemy[i]->attackHit == true) {
						if (_enemy[i]->mode != 3) { continue; }
						Hhit = false;
						_enemy[i]->attackHit = false;
						gateHP -= 1;
						shake = true;
						reception = 600;
					}

					//エネミーからのダメージ
					if (Hhit == true && _enemy[i]->attackHit == true) {
						_enemy[i]->attackHit = false;
						popHp += 10;
					}
					//ゲームオーバー条件				
					//if (gateHP <= 0 || playerHp <= 0) { SceneChange = true; }
					reception--;
				}
			}

			//更新処理(ゲーム)
			{
				//防衛施設操作
				for (int i = 0; i < 6; i++) {
					if (distance(_player->GetPosition(), defense_facilities[i]->position) >= 3) { continue; }
					if (!(input->Trigger(DIK_F) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonB))) { continue; }
					setObjectPos = defense_facilities[i]->position;
					SetNum = i;
					//カメラ注視点を決定
					//カメラとプレイヤーの距離を決定
					const float distanceFromPlayerToCamera = 40.0f;
					//カメラの高さ
					const float cameraHeight = 10.0f;

					//カメラの正面ベクトルを決定
					vv0 = { 0.0f,0.0f,1.0f,0.0f };
					//
					XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

					//カメラ位置を決定
					setObjectPos.y += cameraHeight;
					setCamera->SetEye(setObjectPos);
					XMFLOAT3 _target;
					_target.x = setObjectPos.x - direction.x * distanceFromPlayerToCamera;
					_target.y = setObjectPos.y - direction.y * distanceFromPlayerToCamera;
					_target.z = setObjectPos.z - direction.z * distanceFromPlayerToCamera;
					_target.y += cameraHeight / 3.0f;
					setCamera->SetTarget(_target);
					setCamera->Update();
					Wrapper::SetCamera(setCamera);
					GameModeNum = GameMode::SET;
				}

				//HP減少
				if (popHp != 0) {
					playerHp -= 1;
					popHp--;
				}

				if (input->Trigger(DIK_SPACE)) {
					gateBreak_red->SetShake(true);
					gateBreak_yellow->SetShake(true);
					gateBreak_green->SetShake(true);
				}

				//ゲートダメージ時リアクション
				if (shake) {
					Shake3D(stages[64]->position);
				}

				//ステージ移動
				if (UseStage == GameLocation::BaseCamp && _player->GetPosition().z <= -20.0f) {
					fade->SetFadeIn(true);
					light->SetPointLightActive(0, false);

					//フェードイン
					if (fade->GetFadeIn()) {
						fade->FadeIn();
						if (!fade->GetFadeIn()) {
							fade->SetFadeIn(false);
							fade->SetFadeOut(true);
						}
					}

					//フェードアウト
					if (fade->GetFadeOut()) {
						fade->FadeOut();

						UseStage = GameLocation::BaseStage;
						_player->SetPosition(XMFLOAT3(200.0f, 0.0f, 100.0f));
						_player->SetAngleH(90.0f);
					}
				}

				//施設設置
				if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonB)) {
					for (int i = 0; i < stages.size(); i++) {
						//土台でなければスルー
						if (stages[i]->GetObjectNum() != ObjectType::FounDation) { continue; }

						//プレイヤーと土台の距離を計算
						if (distance(stages[i]->position, _player->GetPosition()) > 5) { continue; }

						//施設が設置されていなければスルー
						if (stages[i]->GetInstallation()) { continue; }

						//施設番号を保存
						UseFoundation = i;

						//同ボタン同一フレーム判定回避用フラグ
						WeaponSelectDo = true;

						//施設設置に移行
						GameModeNum = GameMode::WEAPONSELECT;
					}
				}

				//一時停止
				if (directInput->IsButtonPush(directInput->Button09) || input->Trigger(DIK_ESCAPE)) {
					GameModeNum = GameMode::POSE;
					pose = true;
				}

				if (input->Push(DIK_Q)) {
					GameModeNum = GameMode::OVER;
					fade->SethalfFade(true);
				}
				//ゲームクリア条件
				if (repelCount >= 15) {
					GameModeNum = GameMode::CLEAR;
					fade->SethalfFade(true);
				}
				//ゲームオーバー条件
				if (playerHp <= 0 || gateHP <= 0) {
					GameModeNum = GameMode::OVER;
					fade->SethalfFade(true);
				}
			}

			//パーティクル生成
			static float createTime = 0.2f;
			if (createTime <= 0.0f) {
				//particlemanager->CreateParticle(30, _player->model->position, 0.01f, 0.005f, 10, 5.0f, XMFLOAT4(particleColor));

				createTime = 0.2f;
			}
			createTime -= 1.0f / 60.0f;

		}

		if (GameModeNum == GameMode::WEAPONSELECT) {
			weaponSelect->Update();
			for (int i = 0; i < 3; i++) { weaponSlot[i]->Update(); }
			_player->Update();

			//施設選択
			if (SlotCount == 0) {
				weaponSlot[0]->SetSize({ 165.0f, 165.0f });
				weaponSlot[1]->SetSize({ 150.0f, 150.0f });
				weaponSlot[2]->SetSize({ 150.0f, 150.0f });
			}
			else if (SlotCount == 1) {
				weaponSlot[0]->SetSize({ 150.0f, 150.0f });
				weaponSlot[1]->SetSize({ 165.0f, 165.0f });
				weaponSlot[2]->SetSize({ 150.0f, 150.0f });
			}
			else if (SlotCount == 2) {
				weaponSlot[0]->SetSize({ 150.0f, 150.0f });
				weaponSlot[1]->SetSize({ 150.0f, 150.0f });
				weaponSlot[2]->SetSize({ 165.0f, 165.0f });
			}
			if (directInput->IsButtonPush(DirectInput::RightButton)) { SlotCount += 1; }
			else if (directInput->IsButtonPush(DirectInput::LeftButton)) { SlotCount -= 1; }

			//設置施設決定
			if (directInput->IsButtonPush(DirectInput::ButtonB) && !WeaponSelectDo) {
				//土台に施設を設置
				defense_facilities[WeaponCount]->SetPosition({
					stages[UseFoundation]->position.x,
					10,stages[UseFoundation]->position.z });
				defense_facilities[WeaponCount]->SetAlive(true);
				//施設番号を次に
				WeaponCount += 1;

				//設置状態有効
				stages[UseFoundation]->SetInstallation(true);

				//ゲームに戻る
				GameModeNum = GameMode::NASI;
			}

			if (directInput->IsButtonPush(DirectInput::ButtonA)) {
				GameModeNum = GameMode::NASI;
			}
			WeaponSelectDo = false;
		}
		//防衛施設操作時処理
		if (GameModeNum == GameMode::SET) {
			float angleH = 50.0f;
			float angleV = 5.0f;
			XMMATRIX matRot = XMMatrixIdentity();
			//カメラの正面ベクトルを決定
			vv0 = { 0.0f,0.0f,1.0f,0.0f };

			if (directInput->leftStickX() >= 0.5f || directInput->leftStickX() <= -0.5f) {
				angleHorizonal +=
					XMConvertToRadians(angleH * directInput->getLeftX());
			}

			//角度回転
			matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

			vv0 = XMVector3TransformNormal(vv0, matRot);
			XMFLOAT3 set_v;
			set_v.x = vv0.m128_f32[0];
			set_v.y = vv0.m128_f32[1];
			set_v.z = vv0.m128_f32[2];

			defense_facilities[SetNum]->SetMatRot(LookAtRotation(set_v, XMFLOAT3(0.0f, 1.0f, 0.0f)));
			defense_facilities[SetNum]->SetShotVec(vv0);
			//カメラ注視点を決定
			//カメラとプレイヤーの距離を決定
			const float distanceFromPlayerToCamera = 40.0f;
			//カメラの高さ
			const float cameraHeight = 10.0f;

			//
			XMFLOAT3 direction = { set_v.x ,set_v.y,set_v.z };

			XMFLOAT3 _target;
			_target.x = setObjectPos.x - direction.x * distanceFromPlayerToCamera;
			_target.y = setObjectPos.y - direction.y * distanceFromPlayerToCamera;
			_target.z = setObjectPos.z - direction.z * distanceFromPlayerToCamera;
			_target.y += cameraHeight / 3.0f;
			setCamera->SetTarget(_target);
			Wrapper::SetCamera(setCamera);

			if (input->Trigger(DIK_F) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonX)) {
				Wrapper::SetCamera(mainCamera);
				FbxObject3d::SetCamera(dx12->Camera());
				GameModeNum = GameMode::NASI;
			}
		}
		//ゲームクリア時処理
		if (GameModeNum == GameMode::CLEAR)
		{
			fade->halfFade();
			if (fade->GetAlpha() >= 0.7f) {
				result = true;
			}
			if (result) {
				clear->SetClear(true);
			}

			fade->Update();
			clear->Update();

			//エンド→タイトル遷移
			if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
				SceneNum = TITLE;
				Wrapper::SetCamera(titleCamera);
				clear->SetClear(false);

				GameModeNum = GameMode::START;
				UseStage = GameLocation::BaseCamp;
				fade->Reset();
				repelCount = 0;

				//カメラの移動元の作成
				const float distanceFromPlayerToCamera = 10.0f;//カメラとプレイヤーの距離を決定
				const float cameraHeight = 25.0f;//カメラの高さ
				vv0 = { -1.0f,0.0f,0.0f,0.0f };//カメラの正面ベクトルを決定
				XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

				afterEye.x = _player->GetPosition().x + direction.x * distanceFromPlayerToCamera;
				afterEye.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera;
				afterEye.z = _player->GetPosition().z + direction.z * distanceFromPlayerToCamera;
				afterEye.y += cameraHeight;
				mainCamera->SetEye(afterEye);
				//カメラ注視点を決定
				XMFLOAT3 _target;
				_target.x = _player->GetPosition().x;
				_target.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera * 2.0f;
				_target.z = _player->GetPosition().z;
				//_target.y += cameraHeight / 1.5f;
				mainCamera->SetTarget(_target);
			}
		}
		//ゲームオーバー時処理
		if (GameModeNum == GameMode::OVER) {
			fade->halfFade();
			if (fade->GetAlpha() >= 0.7f) {
				result = true;
			}
			if (result) {
				failed->SetFailed(true);
			}
			fade->Update();
			failed->Update();

			//エンド→タイトル遷移
			if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
				SceneNum = TITLE;
				Wrapper::SetCamera(titleCamera);
				failed->SetFailed(false);

				GameModeNum = GameMode::START;
				UseStage = GameLocation::BaseCamp;
				fade->Reset();
				repelCount = 0;

				//カメラの移動元の作成
				const float distanceFromPlayerToCamera = 10.0f;//カメラとプレイヤーの距離を決定
				const float cameraHeight = 25.0f;//カメラの高さ
				vv0 = { -1.0f,0.0f,0.0f,0.0f };//カメラの正面ベクトルを決定
				XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

				afterEye.x = _player->GetPosition().x + direction.x * distanceFromPlayerToCamera;
				afterEye.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera;
				afterEye.z = _player->GetPosition().z + direction.z * distanceFromPlayerToCamera;
				afterEye.y += cameraHeight;
				mainCamera->SetEye(afterEye);
				//カメラ注視点を決定
				XMFLOAT3 _target;
				_target.x = _player->GetPosition().x;
				_target.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera * 2.0f;
				_target.z = _player->GetPosition().z;
				//_target.y += cameraHeight / 1.5f;
				mainCamera->SetTarget(_target);
			}
		}
		//ポーズ画面時処理
		if (GameModeNum == GameMode::POSE) {
			if (!pose) {
				if (directInput->IsButtonUp(directInput->ButtonPouse) || input->Trigger(DIK_ESCAPE)) {
					GameModeNum = GameMode::NASI;
				}
			}
			pose = false;
		}

		//更新処理(固有)
		{
			if (GameModeNum != GameMode::POSE) {
				Object3Ds::SetCamera(dx12->Camera());
				FbxObject3d::SetCamera(dx12->Camera());

				dx12->SceneUpdate();
				//obj03->Update();
				for (int i = 0; i < 6; i++) {
					defense_facilities[i]->Update();
					defense_facilities[i]->moveUpdate(_enemy);
				}
				//HitBox::mainUpdate(_player->model->GetBoneMat(), _player->model->rotation);

				hp->SetSize(XMFLOAT2(playerHp * 4.5f, 30));
				hp->Update();

				skyDome->SetPosition(XMFLOAT3(_player->GetPosition().x, _player->GetPosition().y + 450.0f, _player->GetPosition().z));
				skyDome->Update();

				particlemanager->Update();
				Bottom->Update();
				for (int i = 0; i < _enemy.size(); i++) {
					light->SetCircleShadowCasterPos(i, XMFLOAT3({ _enemy[i]->GetPosition().x, _enemy[i]->GetPosition().y, _enemy[i]->GetPosition().z }));
					light->SetCircleShadowDir(i, XMVECTOR({ circleShadowDir[0],circleShadowDir[1],circleShadowDir[2],0 }));
					light->SetCircleShadowAtten(i, XMFLOAT3(circleShadowAtten));
					light->SetCircleShadowFacterAngle(i, XMFLOAT2(circleShadowFacterAnlge[1], circleShadowFacterAnlge[2] * _enemy[i]->shadowOffset));
				}
				light->Update();

				moveGuide->Update();

				gateBreak_red->Update();
				gateBreak_yellow->Update();
				gateBreak_green->Update();
			}
		}
	}
}
void GameManager::EndUpdate() {
	//エンド
	if (SceneNum == END) {
		//エンド→タイトル遷移
		if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			SceneNum = TITLE;
		}
	}
}
void GameManager::DebugTestUpdate()
{
	if (SceneNum == Scene::DebugTest) {
		camera->Update();
		particlemanager->Update();
		debugFilde->Update();
		debugCharacter->Update();
		light->Update();
		testModel->Update();
		testObject->Update();

		dx12->SetCamera(camera);

		static float createTime = 0.2f;
		if (createTime <= 0.0f) {

			particlemanager->CreateParticle(30,
				XMFLOAT3(
					debugCharacter->position.x,
					debugCharacter->position.y,
					debugCharacter->position.z),
				0.01f, 0.02f, 64, 1.0f, { 0.2f,0.2f,0.8f,1 }, 1);

			createTime = 0.2f;
		}
		createTime -= 1.0f / 60.0f;

		static XMFLOAT3 pos = { 0,0,0 };
		static float sp = 0.2f;
		if (input->Push(DIK_I)) { pos.z = sp; }
		else if (input->Push(DIK_K)) { pos.z = -sp; }
		else { pos.z = 0; }
		if (input->Push(DIK_J)) { pos.x = -sp; }
		else if (input->Push(DIK_L)) { pos.x = sp; }
		else { pos.x = 0; }
		if (input->Push(DIK_U)) { pos.y = -sp; }
		else if (input->Push(DIK_O)) { pos.y = sp; }
		else { pos.y = 0; }

		debugCharacter->position = add(debugCharacter->position, pos);
	}
}

void GameManager::PlayerUpdate()
{
	if (GameModeNum == GameMode::NASI) {
		_player->Update();
	}

	//武器の位置に当たり判定設置
	weaponCollider.center = XMVectorSet(
		_player->GetPos().x + vv0.m128_f32[0] * 2,
		_player->GetPos().y + 20.0f,
		_player->GetPos().z + vv0.m128_f32[2] * 2,
		1);

	//プレイヤーの正面に当たり判定設置
	playerCollider.center = XMVectorSet(
		_player->GetPos().x,
		_player->GetPos().y + 20.0f,
		_player->GetPos().z,
		1);

	//カメラワーク(追従)
	{
		//カメラとプレイヤーの距離を決定
		const float distanceFromPlayerToCamera = 50.0f;

		//カメラの高さ
		const float cameraHeight = 30.0f;

		//カメラの正面ベクトルを決定
		vv0 = { 0.0f,0.0f,1.0f,0.0f };
		//縦軸の回転
		rotM = XMMatrixRotationX(XMConvertToRadians(_player->GetAngleVertical()));
		vv0 = XMVector3TransformNormal(vv0, rotM);

		//横軸の回転
		rotM = XMMatrixRotationY(XMConvertToRadians(_player->GetAngleHorizonal()));
		vv0 = XMVector3TransformNormal(vv0, rotM);

		//
		XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

		//カメラ位置を決定
		XMFLOAT3 _eye;
		_eye.x = _player->GetPosition().x + direction.x * distanceFromPlayerToCamera;
		_eye.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera;
		_eye.z = _player->GetPosition().z + direction.z * distanceFromPlayerToCamera;
		_eye.y += cameraHeight * 1.3f;
		mainCamera->SetEye(_eye);

		//カメラ注視点を決定
		XMFLOAT3 _target;
		_target.x = _player->GetPosition().x;
		_target.y = _player->GetPosition().y;
		_target.z = _player->GetPosition().z;
		_target.y += cameraHeight;
		mainCamera->SetTarget(_target);
		Wrapper::SetCamera(mainCamera);

		mainCamera->Update();
	}

	//移動
	if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonX) || input->Push(DIK_X)) {
		for (int i = 0; i < sqhere.size(); i++)
		{
			bool Ahit = Collision::CheckSqhere2Sqhere(sqhere[i], weaponCollider);

			if (Ahit != true) { continue; }
			_enemy[i]->damage = true;
			if (_enemy[i]->damage != true)continue;
			_enemy[i]->status.HP -= 1;
			//体力がなくなっていれば
			if (_enemy[i]->status.HP <= 0) {
				repelCount += 1;
			}
		}
	}
}
void GameManager::EnemyUpdate()
{
	//エネミーの生成
	{
		//生成時間になり、生成対象が生きていないなら
		enemyPopTime += 1.0f / 60.0f;
		if (enemyPopTime >= 5.0f) {
			for (int i = 0; i < 3; i++) {
				if (protEnemy[i]->alive) { continue; }

				protEnemy[i]->Appearance();
				_enemy.push_back(protEnemy[i]);

				//当たり判定用球体生成
				Sqhere _sqhere;
				_sqhere.radius = 20.0f;
				sqhere.push_back(_sqhere);

				enemyPopTime = 0.0f;

				break;
			}
		}
	}
	//エネミー関係の制御
	{
		for (int i = 0; i < _enemy.size(); i++) {
			_enemy[i]->moveUpdate(_player->GetPosition(), defense_facilities, stages[64]->position);
			sqhere[i].center = XMVectorSet(_enemy[i]->GetPosition().x, _enemy[i]->GetPosition().y + 10.0f, _enemy[i]->GetPosition().z, 1);
			if (_enemy[i]->alive) { continue; }
			_enemy.erase(_enemy.begin() + i);
			sqhere.erase(sqhere.begin() + i);
		}
	}
}

void GameManager::MainDraw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	if (SceneNum == TITLE) {
		BaseObject::PreDraw(cmdList);
		for (auto& object : titleStages) {
			object->Draw();
		}
		BaseObject::PostDraw();
	}
	else if (SceneNum == GAME) {

		//深度バッファクリア
		dx12->ClearDepthBuffer();

		Sprite::PreDraw(cmdList);
		if (GameModeNum != GameMode::POSE) {
		}
		if (GameModeNum == GameMode::POSE) {
			Pose->Draw();
		}

		Sprite::PostDraw();

		BaseObject::PreDraw(cmdList);

		//プレイヤー描画
		if (GameModeNum != GameMode::SET) {
			_player->Draw(cmdList);
		}
		skyDome->Draw();

		if (UseStage == GameLocation::BaseCamp) {
			//ベースキャンプ描画
			for (auto& object : baseCamp) {
				object->Draw();
			}

			moveGuide->Draw();
		}
		//ステージ描画
		else if (UseStage == GameLocation::BaseStage) {
			for (auto& object : stages) {
				object->Draw();
			}
			//防衛施設描画
			for (int i = 0; i < 6; i++) {
				if (!defense_facilities[i]->GetAlive()) { continue; }
				defense_facilities[i]->Draw();
			}

			//敵描画
			for (int i = 0; i < _enemy.size(); i++) {
				_enemy[i]->Draw(cmdList);
			}
		}

		BaseObject::PostDraw();

		BillboardObject::PreDraw(cmdList);

		Bottom->Draw(cmdList);

		BillboardObject::PostDraw();

		// 3Dオブジェクト描画前処理
		ParticleManager::PreDraw(cmdList);
		// 3Dオブクジェクトの描画
		particlemanager->Draw();

		for (int i = 0; i < _enemy.size(); i++) {
			_enemy[i]->particle->Draw();
		}

		// 3Dオブジェクト描画後処理
		ParticleManager::PostDraw();
	}
	else if (SceneNum == END) {
		Sprite::PreDraw(cmdList);
		End->Draw();
		Sprite::PostDraw();
	}

	else if (SceneNum == Scene::DebugTest) {

		//深度バッファクリア
		dx12->ClearDepthBuffer();

		BaseObject::PreDraw(cmdList);
		debugFilde->Draw();
		testObject->Draw(cmdList);
		for (auto& object : titleStages) {
			object->Draw();
		}
		BaseObject::PostDraw();


		// 3Dオブジェクト描画前処理
		ParticleManager::PreDraw(cmdList);
		// 3Dオブクジェクトの描画
		particlemanager->Draw();

		// 3Dオブジェクト描画後処理
		ParticleManager::PostDraw();
	}
}
void GameManager::SubDraw()
{
	// コマンドリストの取得
	Sprite::PreDraw(dx12->CommandList().Get());

	if (SceneNum == TITLE) {
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
	}
	else if (SceneNum == GAME) {

		//BillboardObject::PreDraw(cmdList);

		//Bottom->Draw(cmdList);

		//BillboardObject::PostDraw();

		if (GameModeNum != GameMode::POSE) {

			if (GameModeNum == GameMode::Preparation) {
				//タイトル
				//Title->Draw();

				//カウント周り
				Start_UI_02.get()->Draw();
				Start_UI_01.get()->Draw();
				Start_UI_03.get()->Draw();
				One_Numbers[one_place].get()->Draw();
				Ten_Numbers[tens_place].get()->Draw();
			}

			hp->Draw();
			HpBer->Draw();


			BreakBar->Draw();
			for (int i = 0; i < repelCount; i++) {
				BreakGage[i]->Draw();
			}

			if (GameModeNum == GameMode::WEAPONSELECT) {
				weaponSelect->Draw();
				for (int i = 0; i < 3; i++) { weaponSlot[i]->Draw(); }
			}
		}
		if (GameModeNum == GameMode::POSE) {
			Pose->Draw();
		}
	}
	else if (SceneNum == END) {
		End->Draw();
	}

	//フェード用画像描画
	start->Draw();
	if (fade->GetFadeIn() || fade->GetFadeOut() || fade->GethalfFade()) {
		fade->Draw();
	}
	if (clear->GetClear()) {
		clear->Draw();
	}
	else if (failed->GetFailed()) {
		failed->Draw();
	}
	if (SceneNum == GAME) {
		if ((GATE_MAX - 3) <= gateHP) {
			gateBreak_green->Draw();
		}
		else if ((GATE_MAX - 6) <= gateHP) {
			gateBreak_yellow->Draw();
		}
		else if ((GATE_MAX - 9) <= gateHP) {
			gateBreak_red->Draw();
		}
	}

	//ローディング中
	if (load) {
		loadResource.get()->Draw();
	}

	Sprite::PostDraw();
}
void GameManager::ShadowDraw(bool isShadow)
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	//深度バッファクリア
	dx12->ClearDepthShadow();

	BaseObject::PreDraw(cmdList);

	if (SceneNum == TITLE) {
		for (auto& object : titleStages) {
			object->ShadowDraw();
		}
	}
	else if (SceneNum == GAME) {
		//プレイヤー描画
		if (GameModeNum != GameMode::SET) {
			_player->ShadowDraw(cmdList);
		}

		if (UseStage == GameLocation::BaseCamp) {
			//ベースキャンプ描画
			for (auto& object : baseCamp) {
				object->ShadowDraw();
			}
		}
		//ステージ描画
		else if (UseStage == GameLocation::BaseStage) {
			for (auto& object : stages) {
				object->ShadowDraw();
			}
			//防衛施設描画
			for (int i = 0; i < 6; i++) {
				if (!defense_facilities[i]->GetAlive()) { continue; }
				defense_facilities[i]->ShadowDraw();
			}

			//敵描画
			for (int i = 0; i < _enemy.size(); i++) {
				_enemy[i]->ShadowDraw(cmdList);
			}
		}
	}

	else if (SceneNum == Scene::DebugTest) {

		debugFilde->ShadowDraw();
		testObject->ShadowDraw(cmdList);
		for (auto& object : titleStages) {
			object->ShadowDraw();
		}
	}
	BaseObject::PostDraw();
}

void GameManager::loading() {

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
void GameManager::asyncLoad()
{
	auto th = std::thread([&] {LoadGameResources(); });

	th.join();

	_loadMode = LoadMode::End;
}

void GameManager::TitleReset()
{
	TitleWave = 0;
}
void GameManager::GameReset()
{
	GameModeNum = GameMode::START;
	UseStage = GameLocation::BaseCamp;
	fade->Reset();
	repelCount = 0;

	//カメラの移動元の作成
	const float distanceFromPlayerToCamera = 10.0f;//カメラとプレイヤーの距離を決定
	const float cameraHeight = 25.0f;//カメラの高さ
	vv0 = { -1.0f,0.0f,0.0f,0.0f };//カメラの正面ベクトルを決定
	XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

	afterEye.x = _player->GetPosition().x + direction.x * distanceFromPlayerToCamera;
	afterEye.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera;
	afterEye.z = _player->GetPosition().z + direction.z * distanceFromPlayerToCamera;
	afterEye.y += cameraHeight;
	mainCamera->SetEye(afterEye);
	//カメラ注視点を決定
	XMFLOAT3 _target;
	_target.x = _player->GetPosition().x;
	_target.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera * 2.0f;
	_target.z = _player->GetPosition().z;
	//_target.y += cameraHeight / 1.5f;
	mainCamera->SetTarget(_target);
}

XMFLOAT3 GameManager::moveCamera(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct)
{
	XMFLOAT3 pos;
	pos.x = pos1.x + ((pos2.x - pos1.x) * pct);
	pos.y = pos1.y + ((pos2.y - pos1.y) * pct);
	pos.z = pos1.z + ((pos2.z - pos1.z) * pct);
	return pos;
}

void GameManager::LoadTitleResources()
{
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
}
void GameManager::LoadGameResources()
{
	if (!Sprite::loadTexture(SpriteName::Numbers, L"Resources/Numbers.png")) { assert(0); }
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

	//スカイドーム-------------------
	skyDome = Object3Ds::Create(skyDomeModel);
	skyDome->scale = { 4,4,4 };
	skyDome->position = { 0,350,0 };

	//ゲーム内ガイド
	guideModels[0] = Model::CreateFromOBJ("guide");
	moveGuide = Object3Ds::Create(guideModels[0]);
	moveGuide->SetPosition(XMFLOAT3(555.0f, 3.0f, 10.0f));
	moveGuide->scale = XMFLOAT3(5, 5, 5);

	//ヒットボックス-----------------
	triangle[0].p0 = XMVectorSet(stages[64]->position.x - 100.0f, stages[64]->position.y, stages[64]->position.z, 1);
	triangle[0].p1 = XMVectorSet(stages[64]->position.x - 100.0f, stages[64]->position.y + 120.0f, stages[64]->position.z, 1);
	triangle[0].p2 = XMVectorSet(stages[64]->position.x + 100.0f, stages[64]->position.y, stages[64]->position.z, 1);
	triangle[0].normal = XMVectorSet(0.0f, 0.0f, 1.0f, 0);

	//防衛砲台
	for (int i = 0; i < 6; i++) {
		defense_facilities[i] = DefCannon::Appearance(0, defenceModel, defenceModel, defenceModel);
		defense_facilities[i]->BulletCreate(bulletModel);
		defense_facilities[i]->Update();
		defense_facilities[i]->scale = { 5,5,5 };
	}

	//プレイヤー---------------------
	_player = Player::Create(testModel);
	_player->CreateWeapon(Model::CreateFromOBJ("weapon"));
	_player->SetScale({ 0.2f, 0.2f, 0.2f });
	_player->SetPosition(XMFLOAT3(0.0f, 0.0f, 100.0f));
	_player->PlayAnimation();

	//当たり判定用球体生成
	weaponCollider.radius = 10.0f;
	playerCollider.radius = 10.0f;

	_player->GetInstance()->GetPos();
	//エネミー-----------------------
	for (int i = 0; i < 3; i++)
	{
		protEnemy[i] = Enemy::Create(wolf[i], golem[i]);
		//パーティクル生成
		protEnemy[i]->Particle();
	}

	//カメラの移動元の作成
	const float distanceFromPlayerToCamera = 10.0f;//カメラとプレイヤーの距離を決定
	const float cameraHeight = 25.0f;//カメラの高さ
	vv0 = { -1.0f,0.0f,0.0f,0.0f };//カメラの正面ベクトルを決定
	XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

	afterEye.x = _player->GetPosition().x + direction.x * distanceFromPlayerToCamera;
	afterEye.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera;
	afterEye.z = _player->GetPosition().z + direction.z * distanceFromPlayerToCamera;
	afterEye.y += cameraHeight;
	mainCamera->SetEye(afterEye);
	//カメラ注視点を決定
	XMFLOAT3 _target;
	_target.x = _player->GetPosition().x;
	_target.y = _player->GetPosition().y + direction.y * distanceFromPlayerToCamera * 2.0f;
	_target.z = _player->GetPosition().z;
	//_target.y += cameraHeight / 1.5f;
	mainCamera->SetTarget(_target);

}
void GameManager::LoadAnotherResourecs()
{
}

XMFLOAT3 add(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	XMFLOAT3 pos;
	pos.x = v1.x + v2.x;
	pos.y = v1.y + v2.y;
	pos.z = v1.z + v2.z;

	return pos;
}