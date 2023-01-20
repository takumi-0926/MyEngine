#include <algorithm>
#include "gameManager.h"
#include "object\baseObject.h"
#include "FBX\FbxLoader.h"
#include "FBX\FbxObject3d.h"
#include "PMD/PMXLoader.h"
#include "stage.h"
#include "Player.h"
#include "JsonLoader.h"

#include "Collision\MeshCollider.h"
#include "Collision\SphereCollider.h"
#include "Collision\CollisionManager.h"

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
	if (!Sprite::loadTexture(5, L"Resources/Damege.png")) {
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

	if (!BillboardObject::LoadTexture(0, L"Resources/GateUI_red.png")) {
		assert(0);
		return false;
	}

	//カメラをセット
	camera = new DebugCamera(Application::window_width, Application::window_height, input);
	mainCamera = new Camera(Application::window_width, Application::window_height);
	setCamera = new Camera(Application::window_width, Application::window_height);
	camera->Update();

	Wrapper::SetCamera(mainCamera);
	dx12->SceneUpdate();

	//光源
	light = Light::Create();
	light->SetDirLightActive(0, true);
	light->SetDirLightActive(1, true);
	light->SetDirLightActive(2, false);
	light->SetPointLightActive(0, true);
	light->SetPointLightAtten(0, XMFLOAT3(0.0001f, 0.0001f, 0.0001f));
	light->SetPointLightActive(1, false);
	light->SetPointLightActive(2, false);
	light->SetCircleShadowActive(0, true);
	light->SetCircleShadowActive(1, true);
	light->SetCircleShadowActive(2, true);
	//ライトセット
	Wrapper::SetLight(light);

	particlemanager = ParticleManager::Create();
	particlemanager->Update();

	Bottom = BillboardObject::Create(0);
	Bottom->Update();

	FbxObject3d::SetDevice(dx12->GetDevice());
	FbxObject3d::SetCamera(dx12->Camera());
	FbxObject3d::CreateGraphicsPipeline();

	//基本オブジェクト--------------
	defenceModel = Model::CreateFromOBJ("KSR-29");
	skyDomeModel = Model::CreateFromOBJ("skydome");
	bulletModel = Model::CreateFromOBJ("bullet");
	Box1x1 = Model::CreateFromOBJ("Box");

	golem = FbxLoader::GetInstance()->LoadModelFromFile("Golem");
	wolf = FbxLoader::GetInstance()->LoadModelFromFile("Wolf");

	//ステージデータ及びモデルデータ読み込み
	stageData = JsonLoader::LoadJsonFile("stageData");
	stageModels.insert(std::make_pair("Ground", Model::CreateFromOBJ("Ground")));
	stageModels.insert(std::make_pair("Gate", Model::CreateFromOBJ("Gate")));
	stageModels.insert(std::make_pair("Wall", Model::CreateFromOBJ("Wall")));
	stageModels.insert(std::make_pair("Tree", Model::CreateFromOBJ("Tree")));
	stageModels.insert(std::make_pair("Cliff", Model::CreateFromOBJ("Cliff")));
	stageModels.insert(std::make_pair("Foundation", Model::CreateFromOBJ("Foundation")));
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

		stages.push_back(newObject);
	}
	//一度リセット（ベースキャンプの読み込みのため）
	stageData->objects = {};
	baseCampData = JsonLoader::LoadJsonFile("BaseCampData");
	stageModels.insert(std::make_pair("WallRock", Model::CreateFromOBJ("WallRock")));
	stageModels.insert(std::make_pair("Camp", Model::CreateFromOBJ("Camp")));
	stageModels.insert(std::make_pair("Tent", Model::CreateFromOBJ("Tent")));
	for (auto& objectData : baseCampData->objects) {
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

	//防衛砲台
	for (int i = 0; i < 6; i++) {
		defense_facilities[i] = DefCannon::Appearance(0, defenceModel, defenceModel, defenceModel);
		defense_facilities[i]->BulletCreate(bulletModel);
		defense_facilities[i]->Update();
		defense_facilities[i]->scale = { 5,5,5 };
	}

	//スカイドーム-------------------
	skyDome = Object3Ds::Create(skyDomeModel);
	skyDome->scale = { 7,7,7 };
	skyDome->position = { 0,350,0 };

	//MMDオブジェクト----------------
	modelPlayer = PMDmodel::CreateFromPMD("Resources/Model/初音ミク.pmd");
	modelPlayer->LoadVMDFile(vmdData::WAIT, "Resources/vmd/marieru_stand.vmd");
	modelPlayer->LoadVMDFile(vmdData::WALK, "Resources/vmd/Rick式走りモーション02.vmd");
	modelPlayer->LoadVMDFile(vmdData::ATTACK, "Resources/vmd/attack.vmd");
	modelPlayer->LoadVMDFile(vmdData::DAMAGE, "Resources/vmd/腹部ダメージモーション.vmd");
	modelPlayer->LoadVMDFile(vmdData::AVOID, "Resources/vmd/Rick式走りモーション05.vmd");

	//プレイヤー---------------------
	_player = Player::Create(modelPlayer);
	_player->SetInput(*input);
	_player->model->scale = { 1,1,1 };
	_player->model->position = { 548.0f,0,196.0f };
	_player->model->playAnimation();
	_player->model->animation = true;

	//エネミー-----------------------
	protEnemy[0] = Enemy::Create(wolf, golem);
	protEnemy[1] = Enemy::Create(wolf, golem);
	protEnemy[2] = Enemy::Create(wolf, golem);

	//スプライト---------------------
	Title = Sprite::Create(0, { 0.0f,0.0f });
	End = Sprite::Create(1, { 0.0f,0.0f });
	HpBer = Sprite::Create(3, { 0.0f,0.0f });
	Pose = Sprite::Create(15, { 0.0f,0.0f });

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

	//ヒットボックス-----------------
	HitBox::CreatePipeline(dx12);
	HitBox::CreateTransform();
	HitBox::CreateHitBox(_player->model->GetBonePos("頭先"), defenceModel);
	HitBox::hitBox[0]->scale = XMFLOAT3(5, 10, 5);
	triangle[0].p0 = XMVectorSet(stages[66]->position.x - 100.0f, stages[66]->position.y, stages[66]->position.z, 1);
	triangle[0].p1 = XMVectorSet(stages[66]->position.x - 100.0f, stages[66]->position.y + 120.0f, stages[66]->position.z, 1);
	triangle[0].p2 = XMVectorSet(stages[66]->position.x + 100.0f, stages[66]->position.y, stages[66]->position.z, 1);
	triangle[0].normal = XMVectorSet(0.0f, 0.0f, 1.0f, 0);

	//入力及び音声
	input->Update();
	audio->Load();

	SceneNum = TITLE;

	//カメラの移動元の作成
	const float distanceFromPlayerToCamera = 10.0f;//カメラとプレイヤーの距離を決定
	const float cameraHeight = 25.0f;//カメラの高さ
	vv0 = { -1.0f,0.0f,0.0f,0.0f };//カメラの正面ベクトルを決定
	XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

	afterEye.x = _player->model->position.x + direction.x * distanceFromPlayerToCamera;
	afterEye.y = _player->model->position.y + direction.y * distanceFromPlayerToCamera;
	afterEye.z = _player->model->position.z + direction.z * distanceFromPlayerToCamera;
	afterEye.y += cameraHeight;
	mainCamera->SetEye(afterEye);
	//カメラ注視点を決定
	XMFLOAT3 _target;
	_target.x = _player->model->position.x - direction.x * distanceFromPlayerToCamera;
	_target.y = _player->model->position.y - direction.y * distanceFromPlayerToCamera;
	_target.z = _player->model->position.z - direction.z * distanceFromPlayerToCamera;
	_target.y += cameraHeight / 1.5f;
	mainCamera->SetTarget(_target);

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
		ImGui::Text("PlayerPosition : %.2f %.2f", _player->model->position.x, _player->model->position.z);
		ImGui::Text("ClearResultPos : %.2f %.2f", clear->Pos().x, clear->Pos().y);
		ImGui::Checkbox("EnemyPop", &blnChk);
		ImGui::Checkbox("test", &_player->model->a);
		ImGui::RadioButton("Game Camera", &radio, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Debug Camera", &radio, 1);
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

		ImGui::End();

		ImGui::Begin("Particle");
		ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);
		ImGui::ColorPicker4("ColorPicker4", particleColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
		ImGui::End();

		//カメラ切り替え
		static bool isCamera = false;
		if (radio == 0 && isCamera == false) {
			Wrapper::SetCamera(mainCamera);
			FbxObject3d::SetCamera(dx12->Camera());

			isCamera = true;
		}
		else if (radio == 1 && isCamera == true) {
			Wrapper::SetCamera(camera);
			FbxObject3d::SetCamera(dx12->Camera());

			isCamera = false;
		}
	}

	//ライト
	light->SetPointLightPos(0, XMFLOAT3(_player->model->position.x, _player->model->position.y + 20.0f, _player->model->position.z));

	if (input->Push(DIK_I)) { clear->MovePos(XMFLOAT2(0, 1)); }
	if (input->Push(DIK_K)) { clear->MovePos(XMFLOAT2(0, -1)); }
	if (input->Push(DIK_J)) { clear->MovePos(XMFLOAT2(-1, 0)); }
	if (input->Push(DIK_L)) { clear->MovePos(XMFLOAT2(1, 0)); }

	TitleUpdate();
	GameUpdate();
	EndUpdate();
}
void GameManager::TitleUpdate()
{
	//タイトル更新
	if (SceneNum == TITLE) {

		if (resetFlag == false) {
			//Initalize(dx12, audio, input);
			resetFlag = true;
		}

		if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			Title->Update();
			fade->SetFadeIn(true);
			start->SetFadeIn(true);
		}
		//フェードイン
		if (fade->GetFadeIn()) {
			fade->FadeIn();
			if (!fade->GetFadeIn()) {
				SceneNum = GAME;
				fade->SetFadeIn(false);
				fade->SetFadeOut(true);
			}
		}

		debugText.Print("hello", 100.0f, 100.0f, 8.0f);

	}
}
void GameManager::GameUpdate() {
	//ゲーム
	if (SceneNum == GAME) {

		//フェードアウト
		if (fade->GetFadeOut()) {
			fade->FadeOut();
		}

		//当たり判定確認
		CollisionManager::GetInstance()->CheckAllCollision();

		//ゲームスタート時処理
		if (GameModeNum == GameMode::START) {

			//カメラの移動先の作成
			static float step = 0;
			const float distanceFromPlayerToCamera = 40.0f;//カメラとプレイヤーの距離を決定
			const float cameraHeight = 25.0f;//カメラの高さ
			vv0 = { 0.0f,0.0f,1.0f,0.0f };//カメラの正面ベクトルを決定
			XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

			XMFLOAT3 beforeEye;
			beforeEye.x = _player->model->position.x + direction.x * distanceFromPlayerToCamera;
			beforeEye.y = _player->model->position.y + direction.y * distanceFromPlayerToCamera;
			beforeEye.z = _player->model->position.z + direction.z * distanceFromPlayerToCamera;
			beforeEye.y += cameraHeight;

			//カメラ注視点を決定
			XMFLOAT3 _target;
			_target.x = _player->model->position.x - direction.x * distanceFromPlayerToCamera;
			_target.y = _player->model->position.y - direction.y * distanceFromPlayerToCamera;
			_target.z = _player->model->position.z - direction.z * distanceFromPlayerToCamera;
			_target.y += cameraHeight / 1.5f;
			mainCamera->SetTarget(_target);
			mainCamera->SetEye(moveCamera(mainCamera->GetEye(), beforeEye, step += 0.0005f));
			XMFLOAT3 e = mainCamera->GetEye();
			if (samePoint(e, beforeEye)) {
				GameModeNum = GameMode::NASI;
			}

			//スタート時画像表示
			start->SlideIn();
			start->FadeIn();
			start->SlideOut();
			if (start->GetSlideOut()) {
				start->FadeOut();
				if (!start->GetFadeOut()) {
					GameModeNum = GameMode::NASI;
				}
			}
			start->Update();
		}
		//ゲーム進行中処理
		if (GameModeNum == GameMode::NASI) {
			//エネミーの生成
			{
				//生成時間になり、生成対象が生きていないなら
				if (enemyPopTime >= 1.0f && protEnemy[enemyNum]->alive == false) {

					protEnemy[enemyNum]->Appearance();
					//武器生成
					//protEnemy[enemyNum]->CreateWeapon(Model::CreateFromOBJ("weapon"));
					//パーティクル生成
					protEnemy[enemyNum]->CreateParticle();

					_enemy.push_back(protEnemy[enemyNum]);

					//当たり判定用球体生成
					Sqhere _sqhere;
					_sqhere.radius = 5.0f;
					_sqhere.center = XMVectorSet(_enemy[enemyNum]->GetPosition().x, _enemy[enemyNum]->GetPosition().y, _enemy[enemyNum]->GetPosition().z, 1);
					sqhere.push_back(_sqhere);

					enemyNum++;
					if (enemyNum >= 3) { enemyNum = 0; }
				}
				enemyPopTime += 1.0f / 60.0f;
				//三体まで
				//if (_enemy.size() <= 2) {
				//	if (useModel >= 3) { useModel = 0; }
					//ene = Enemy::Appearance(golem[useModel], wolf[useModel]);
					//if (ene != nullptr) {
					//	//格納
					//	_enemy.push_back(ene);
					//	useModel += 1;
				//	}
				//}
			}
			//エネミー関係の制御
			{
				for (int i = 0; i < _enemy.size(); i++) {
					_enemy[i]->moveUpdate(_player->model->position, defense_facilities, stages[66]->position);
					sqhere[i].center = XMVectorSet(_enemy[i]->GetPosition().x, _enemy[i]->GetPosition().y, _enemy[i]->GetPosition().z, 1);
					if (_enemy[i]->alive == true) { continue; }
					_enemy.erase(_enemy.begin());
					sqhere.erase(sqhere.begin());
				}
			}
			//当たり判定（プレイヤー / 敵 / 最終関門）
			{
				for (int i = 0; i < sqhere.size(); i++) {
					for (int j = 0; j < HitBox::_hit.size(); j++) {
						bool Hhit = Collision::CheckSqhere2Sqhere(sqhere[i], HitBox::_hit[j]);
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
							//_player->model->vmdNumber = vmdData::DAMAGE;
						}
						//ゲームオーバー条件				
						//if (gateHP <= 0 || playerHp <= 0) { SceneChange = true; }
						reception--;
					}
				}
			}

			//移動
			{
				//移動ベクトル
				XMFLOAT3 v = { (directInput->getLeftX()),0.0f,-(directInput->getLeftY()) };
				HitBox::hitBox[0]->position = XMFLOAT3(
					_player->model->position.x,
					_player->model->position.y + 10.0f,
					_player->model->position.z);

				if (_player->model->oldVmdNumber != vmdData::ATTACK) { _player->model->oldVmdNumber = _player->model->vmdNumber; }
				else if (_player->model->oldVmdNumber != vmdData::DAMAGE) { _player->model->oldVmdNumber = _player->model->vmdNumber; }
				if (directInput->leftStickX() < 0.0f || directInput->leftStickX() > 0.0f || directInput->leftStickY() < 0.0f || directInput->leftStickY() > 0.0f) {
					_player->model->vmdNumber = vmdData::WALK;
					if (_player->GetAction() == action::Avoid) { _player->model->vmdNumber = vmdData::AVOID; }
					HitBox::hitBox[0]->scale = XMFLOAT3(5, 10, 5);
					HitBox::_hit[0].radius = 5;

					if (directInput->getTriggerZ() != 0) {
						speed = 2.0f;
					}
					else { speed = 1.0f; }
					//左移動
					if (input->Push(DIK_A) || directInput->leftStickX() < 0.0f) {
						_player->model->position = (MoveLeft(_player->model->position));
						for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveLeft(HitBox::GetHit()[i]->position); }
					}
					//右移動
					if (input->Push(DIK_D) || directInput->leftStickX() > 0.0f) {
						_player->model->position = (MoveRight(_player->model->position));
						for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveRight(HitBox::GetHit()[i]->position); }
					}
					//下移動
					if (input->Push(DIK_W) || directInput->leftStickY() < 0.0f) {
						_player->model->position = (MoveBefore(_player->model->position));
						for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveBefore(HitBox::GetHit()[i]->position); }
					}
					//上移動
					if (input->Push(DIK_S) || directInput->leftStickY() > 0.0f) {
						_player->model->position = (MoveAfter(_player->model->position));
						for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveAfter(HitBox::GetHit()[i]->position); }
					}
					XMMATRIX matRot = XMMatrixIdentity();
					//角度回転
					matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

					XMVECTOR _v({ v.x, v.y, v.z, 0 });
					_v = XMVector3TransformNormal(_v, matRot);
					v.x = _v.m128_f32[0];
					v.y = _v.m128_f32[1];
					v.z = _v.m128_f32[2];

					_player->model->SetMatRot(LookAtRotation(v, XMFLOAT3(0.0f, 1.0f, 0.0f)));
					if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA) || input->Push(DIK_Z)) {
						_player->model->vmdNumber = vmdData::AVOID;
						_player->SetAction(action::Avoid);
						_player->SetAvoidVec(v);
					}

				}
				else if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonX) || input->Push(DIK_X)) {
					_player->model->vmdNumber = vmdData::ATTACK;
					HitBox::hitBox[0]->scale = XMFLOAT3(10, 10, 10);
					HitBox::_hit[0].radius = 10;
					for (int i = 0; i < sqhere.size(); i++)
					{
						bool Ahit = Collision::CheckSqhere2Sqhere(sqhere[i], HitBox::_hit[0]);

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
				else {
					_player->model->vmdNumber = vmdData::WAIT;
					HitBox::hitBox[0]->scale = XMFLOAT3(5, 10, 5);
					HitBox::_hit[0].radius = 5;
				}
			}
			//カメラワーク(プレイヤー)
			float angleH = 100.0f;
			float angleV = 40.0f;
			if (directInput->rightStickX() >= 0.5f || directInput->rightStickX() <= -0.5f) {
				angleHorizonal +=
					XMConvertToRadians(angleH * directInput->getRightX());
			}
			if (directInput->rightStickY() >= 0.5f || directInput->rightStickY() <= -0.5f) {
				angleVertical +=
					XMConvertToRadians(angleV * directInput->getRightY());
				//制限角度
				if (angleVertical >= 20) {
					angleVertical = 20;
				}
				//制限角度
				if (angleVertical <= -20) {
					angleVertical = -20;
				}
			}
			if (input->Push(DIK_RIGHT) || input->Push(DIK_LEFT) || input->Push(DIK_UP) || input->Push(DIK_DOWN)) {
				//右
				if (input->Push(DIK_RIGHT)) {
					angleHorizonal +=
						XMConvertToRadians(angleH * directInput->getRightX());
				}
				//左
				if (input->Push(DIK_LEFT)) {
					angleHorizonal -=
						XMConvertToRadians(angleH);
				}
				//上
				if (input->Push(DIK_UP)) {
					angleVertical -=
						XMConvertToRadians(angleV);
					//制限角度
					if (angleVertical <= -20) {
						angleVertical = -20;
					}
				}
				//下
				if (input->Push(DIK_DOWN)) {
					angleVertical +=
						XMConvertToRadians(angleV);
					//制限角度
					if (angleVertical >= 20) {
						angleVertical = 20;
					}
				}
			}
			//カメラワーク(追従)
			{
				//カメラとプレイヤーの距離を決定
				const float distanceFromPlayerToCamera = 40.0f;

				//カメラの高さ
				const float cameraHeight = 25.0f;

				//カメラの正面ベクトルを決定
				vv0 = { 0.0f,0.0f,1.0f,0.0f };

				//縦軸の回転
				rotM = XMMatrixRotationX(XMConvertToRadians(angleVertical));
				vv0 = XMVector3TransformNormal(vv0, rotM);

				//横軸の回転
				rotM = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));
				vv0 = XMVector3TransformNormal(vv0, rotM);

				//
				XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

				//カメラ位置を決定
				XMFLOAT3 _eye;
				_eye.x = _player->model->position.x + direction.x * distanceFromPlayerToCamera;
				_eye.y = _player->model->position.y + direction.y * distanceFromPlayerToCamera;
				_eye.z = _player->model->position.z + direction.z * distanceFromPlayerToCamera;
				_eye.y += cameraHeight;
				mainCamera->SetEye(_eye);

				//カメラ注視点を決定
				XMFLOAT3 _target;
				_target.x = _player->model->position.x - direction.x * distanceFromPlayerToCamera;
				_target.y = _player->model->position.y - direction.y * distanceFromPlayerToCamera;
				_target.z = _player->model->position.z - direction.z * distanceFromPlayerToCamera;
				_target.y += cameraHeight / 1.5f;
				mainCamera->SetTarget(_target);

				mainCamera->Update();
			}
			//更新処理(ゲーム)
			{
				//防衛施設操作
				for (int i = 0; i < 6; i++) {
					if (distance(_player->model->position, defense_facilities[i]->position) >= 3) { continue; }
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
					FbxObject3d::SetCamera(setCamera);
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
					Shake3D(stages[66]->position);
				}

				//ステージ移動
				if (UseStage == GameLocation::BaseCamp && _player->model->position.z <= 5.0f) {
					UseStage = GameLocation::BaseStage;
					_player->model->SetPosition(XMFLOAT3(225.0f, 0.0f, 275.0f));
					angleHorizonal = 90.0f;
				}

				//施設設置
				if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonB)) {
					for (int i = 0; i < stages.size(); i++) {
						//土台でなければスルー
						if (stages[i]->GetObjectNum() != ObjectType::FounDation) { continue; }

						//プレイヤーと土台の距離を計算
						if (distance(stages[i]->position, _player->model->position) > 5) { continue; }

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

				if (SceneChange) {
					SceneNum = END;
				}
			}

			//パーティクル生成
			static float createTime = 0.2f;
			if (createTime <= 0.0f) {
				particlemanager->CreateParticle(_player->model->position, 10, XMFLOAT4(particleColor));
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
			setCamera->Update();

			if (input->Trigger(DIK_F) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonX)) {
				Wrapper::SetCamera(mainCamera);
				FbxObject3d::SetCamera(dx12->Camera());
				GameModeNum = GameMode::NASI;
			}
		}
		//ゲームクリア時処理
		if (GameModeNum == GameMode::CLEAR) {
			fade->halfFade();
			if (fade->GetAlpha() >= 0.7f) {
				result = true;
			}
			if (result) {
				clear->SetClear(true);
			}

			fade->Update();
			clear->Update();
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

				dx12->SceneUpdate();
				camera->Update();
				mainCamera->Update();
				//obj03->Update();
				_player->Update();
				_player->SetInput(*input);
				for (int i = 0; i < _enemy.size(); i++) {
					_enemy[i]->Update();
				}
				for (int i = 0; i < 6; i++) {
					defense_facilities[i]->Update();
					defense_facilities[i]->moveUpdate(_enemy);
				}
				HitBox::mainUpdate(_player->model->GetBoneMat(), _player->model->rotation);

				hp->SetSize(XMFLOAT2(playerHp * 4.5f, 30));
				hp->Update();

				skyDome->SetPosition(XMFLOAT3(_player->model->position.x, _player->model->position.y + 450.0f, _player->model->position.z));
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

				gateBreak_red->Update();
				gateBreak_yellow->Update();
				gateBreak_green->Update();
			}
		}

		//ステージ描画更新処理
		for (auto& object : stages) {
			object->Update();
		}
		for (auto& object : baseCamp) {
			object->Update();
		}
	}
}
void GameManager::EndUpdate() {
	//エンド
	if (SceneNum == END) {
		//エンド→タイトル遷移
		if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			SceneNum = TITLE;
			Title->Update();
		}
	}
}

void GameManager::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	if (SceneNum == TITLE) {
		Sprite::PreDraw(cmdList);
		Title->Draw();
		debugText.DrawAll(cmdList);
		Sprite::PostDraw();

		//BaseObject::PreDraw(cmdList);
		//BaseObject::PostDraw();

	}
	else if (SceneNum == GAME) {
		Sprite::PreDraw(cmdList);
		Sprite::PostDraw();

		//深度バッファクリア
		dx12->ClearDepthBuffer();

		Bottom->Draw(cmdList);

		BaseObject::PreDraw(cmdList);

		//プレイヤー描画
		if (GameModeNum != GameMode::SET) {
			_player->Draw();
		}
		skyDome->Draw();

		if (UseStage == GameLocation::BaseCamp) {
			//ベースキャンプ描画
			for (auto& object : baseCamp) {
				object->Draw();
			}
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

		// 3Dオブジェクト描画前処理
		ParticleManager::PreDraw(cmdList);
		// 3Dオブクジェクトの描画
		particlemanager->Draw();
		// 3Dオブジェクト描画後処理
		ParticleManager::PostDraw();

		Sprite::PreDraw(cmdList);
		if (GameModeNum != GameMode::POSE) {

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

		Sprite::PostDraw();
	}
	else if (SceneNum == END) {
		Sprite::PreDraw(cmdList);
		End->Draw();
		Sprite::PostDraw();
	}

	//フェード用画像描画
	Sprite::PreDraw(cmdList);
	if (fade->GetFadeIn() || fade->GetFadeOut() || fade->GethalfFade()) {
		fade->Draw();
	}
	if (clear->GetClear()) {
		clear->Draw();
	}
	else if (failed->GetFailed()) {
		failed->Draw();
	}
	start->Draw();
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
	Sprite::PostDraw();
}

XMFLOAT3 GameManager::moveCamera(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct)
{
	XMFLOAT3 pos;
	pos.x = pos1.x + ((pos2.x - pos1.x) * pct);
	pos.y = pos1.y + ((pos2.y - pos1.y) * pct);
	pos.z = pos1.z + ((pos2.z - pos1.z) * pct);
	return pos;
}
