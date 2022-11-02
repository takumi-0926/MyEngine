#include <algorithm>
#include "gameManager.h"
#include "object\baseObject.h"
#include "FBX\FbxLoader.h"
#include "FBX\FbxObject3d.h"
#include "PMD/PMXLoader.h"
#include "stage.h"
#include "Player.h"

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

	//this->audio->Load();
	//画像リソース
	if (!Sprite::loadTexture(0, L"Resources/Title.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(1, L"Resources/haikei.png")) {
		assert(0);
		return false;
	}
	if (!Sprite::loadTexture(2, L"Resources/end.png")) {
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

	//カメラをセット
	camera = new DebugCamera(Application::window_width, Application::window_height, input);
	mainCamera = new Camera(Application::window_width, Application::window_height);
	camera->SetTarget({ 0,20,0 });
	camera->SetDistance(80.0f);
	camera->SetUp({ 0, 0, 1 });
	camera->SetEye({ 0,100,0 });
	camera->Update();

	Wrapper::SetCamera(mainCamera);
	dx12->SceneUpdate();

	//ライトセット
	light = light->Create();
	light->SetLightColor({ 1,1,1 });
	Wrapper::SetLight(light);

	//基本オブジェクト--------------
	model01 = Model::CreateFromOBJ("Stone");
	model02 = Model::CreateFromOBJ("FieldStage");
	model03 = Model::CreateFromOBJ("Cannon");
	model04 = Model::CreateFromOBJ("Gate");
	model05 = Model::CreateFromOBJ("Wolf");
	model06 = Model::CreateFromOBJ("maru");

	//modelPlane = Model::CreateFromOBJ("");
	//modelBox = Model::CreateFromOBJ("");
	//modelPyramid = Model::CreateFromOBJ("");
	//Model* modelTable[10] = {
	//	modelPlane,modelPlane,
	//	modelPlane,modelPlane,
	//	modelPlane,modelPlane,
	//	modelPlane,modelPlane,
	//	modelBox,modelPyramid,
	//};

	//全体ステージ
	stage = Stage::Create(model02);
	stage->scale = { 100,100,100 };
	stage->rotation.y = -90;
	stage->SetPosition({ 0.0f,-0.01f,0.0f });

	//const int DIV_NUM = 10;
	//const float LAND_SCALE = 3.0f;
	//for (int i = 0; i < DIV_NUM; i++) {
	//	for (int j = 0; j < DIV_NUM; j++) {
	//		int modelIndex = rand() % 10;

	//	}
	//}

	//ゲート（最終関門）
	obj03 = Object3Ds::Create();
	obj03->SetModel(model04);
	obj03->Update();
	obj03->scale = { 50,50,50 };
	obj03->SetPosition({ 0,0,330 });

	//防衛砲台
	for (int i = 0; i < 6; i++)
	{
		cannon[i] = DefCannon::Create();
		cannon[i]->SetModel(model03);
		cannon[i]->BulletCreate(model06);
		cannon[i]->Update();
		cannon[i]->scale = { 10,10,10 };
	}
	cannon[0]->SetPosition({ -80,0,  130 });
	cannon[1]->SetPosition({ 80,0,  130 });
	cannon[2]->SetPosition({ 80,0,  230 });
	cannon[3]->SetPosition({ -80,0,  230 });
	cannon[4]->SetPosition({ 40,0,  180 });
	cannon[5]->SetPosition({ -40,0,  180 });

	//MMDオブジェクト----------------
	//pmdObject.reset(new PMDobject(dx12));
	//pmdModel.reset(new PMDmodel(dx12, "Resources/Model/初音ミクmetal.pmd", *pmdObject));
	//pmdModel->scale = { 1,1,1 };
	//pmdModel->SetPosition({ 0,0,0 });
	//pmdModel->playAnimation();
	//pmdModel->animation = true;

	modelPlayer = PMDmodel::CreateFromPMD("Resources/Model/初音ミク.pmd");
	modelPlayer->LoadVMDFile(vmdData::WAIT, "Resources/vmd/squat.vmd");
	modelPlayer->LoadVMDFile(vmdData::WALK, "Resources/vmd/Rick式走りモーション02.vmd");
	modelPlayer->LoadVMDFile(vmdData::ATTACK, "Resources/vmd/test.vmd");
	modelPlayer->LoadVMDFile(vmdData::DAMAGE, "Resources/vmd/腹部ダメージモーション.vmd");

	//model->playAnimation();
	//model->animation = true;


	_player = Player::Create(modelPlayer);
	_player->model->scale = { 1,1,1 };
	_player->model->position = { 0,0,0 };
	_player->model->playAnimation();
	_player->model->animation = true;
	//player = PMDobject::Create(modelPlayer);
	//player->model->scale = { 1,1,1 };
	//player->model->position = { 0,0,0 };
	//player->model->playAnimation();
	//player->model->animation = true;

	//スプライト---------------------
	sprite01 = Sprite::Create(0, { 0.0f,0.0f });
	sprite02 = Sprite::Create(1, { 0.0f,0.0f });
	sprite03 = Sprite::Create(2, { 0.0f,0.0f });
	sprite04 = Sprite::Create(3, { 0.0f,0.0f });

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
	fade = Fade::Create(9, { 0.0f,0.0f });
	fade->SetAlpha(0.0f);
	clear = Fade::Create(7, { Application::window_width / 2,Application::window_height / 2 });
	clear->SetAnchorPoint({ 0.5f,0.5f });
	clear->SetSize({ 480,480 });
	failed = Fade::Create(8, { Application::window_width / 2,Application::window_height / 2 });
	failed->SetAnchorPoint({ 0.5f,0.5f });
	failed->SetSize({ 480,480 });
	start = Fade::Create(6, { 0,80 });
	start->SetAnchorPoint({ 0.5f,0.5f });
	start->SetAlpha(0.0f);
	start->SetSize({ 360,360 });
	start->Update();

	//ヒットボックス-----------------
	HitBox::CreatePipeline(dx12);
	HitBox::CreateTransform();
	HitBox::CreateHitBox(_player->model->GetBonePos("頭先"), model01);
	HitBox::hitBox[0]->scale = XMFLOAT3(5, 10, 5);
	triangle[0].p0 = XMVectorSet(obj03->position.x - 100.0f, obj03->position.y, obj03->position.z, 1);
	triangle[0].p1 = XMVectorSet(obj03->position.x - 100.0f, obj03->position.y + 120.0f, obj03->position.z, 1);
	triangle[0].p2 = XMVectorSet(obj03->position.x + 100.0f, obj03->position.y, obj03->position.z, 1);
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
	if (input->Trigger(DIK_M)) {
		pmdModel->playAnimation();
	}
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
		ImGui::RadioButton("Radio 3", &radio, 2);
		int nSlider = 0;
		ImGui::SliderFloat("Enemy Speed", &eneSpeed, 0.05f, 1.0f);
		static float col3[3] = {};
		ImGui::ColorPicker3("ColorPicker3", col3, ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
		static float col4[4] = {};
		ImGui::ColorPicker4("ColorPicker4", col4, ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
		ImGui::End();

		//カメラ切り替え
		static bool isCamera = false;
		if (radio == 0 && isCamera == false) {
			Wrapper::SetCamera(mainCamera);
			isCamera = true;
		}
		else if (radio == 1 && isCamera == true) {
			Wrapper::SetCamera(camera);
			isCamera = false;
		}
	}

	//ライト
	static XMVECTOR lightDir = { 0,1,5,0 };
	{
		if (input->Push(DIK_T)) { lightDir.m128_f32[1] += 1.0f; }
		if (input->Push(DIK_G)) { lightDir.m128_f32[1] -= 1.0f; }
		if (input->Push(DIK_H)) { lightDir.m128_f32[0] += 1.0f; }
		if (input->Push(DIK_F)) { lightDir.m128_f32[0] -= 1.0f; }

		light->SetLightDir(lightDir);
	}

	if (input->Push(DIK_I)) { clear->MovePos(XMFLOAT2(0, 1)); }
	if (input->Push(DIK_K)) { clear->MovePos(XMFLOAT2(0, -1)); }
	if (input->Push(DIK_J)) { clear->MovePos(XMFLOAT2(-1, 0)); }
	if (input->Push(DIK_L)) { clear->MovePos(XMFLOAT2(1, 0)); }

	TitleUpdate();
	GameUpdate();
	EndUpdate();
}
void GameManager::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	if (SceneNum == TITLE) {
		Sprite::PreDraw(cmdList);
		sprite01->Draw();
		Sprite::PostDraw();
	}
	else if (SceneNum == GAME) {
		Sprite::PreDraw(cmdList);
		Sprite::PostDraw();

		//深度バッファクリア
		dx12->ClearDepthBuffer();

		BaseObject::PreDraw(cmdList);
		stage->Draw();
		obj03->Draw();
		//for (int i = 0; i < 6; i++) {
		//	cannon[i]->Draw();
		//}
		for (int i = 0; i < _enemy.size(); i++) {
			//if (_enemy[i]->damage == true) {
			//	_enemy[i]->damegeCount += 1.0f / 60.0f;
			//	if (_enemy[i]->damegeCount >= 0.1f) {
			//		_enemy[i]->damage = false;
			//		_enemy[i]->damegeCount = 0;
			//	}
			//	continue;
			//}

			_enemy[i]->Draw();

		}
		//player->Draw();
		_player->Draw();

		BaseObject::PostDraw();

		Sprite::PreDraw(cmdList);
		hp->Draw();

		sprite04->Draw();

		BreakBar->Draw();
		for (int i = 0; i < repelCount; i++) {
			BreakGage[i]->Draw();
		}

		Sprite::PostDraw();
	}
	else if (SceneNum == END) {
		Sprite::PreDraw(cmdList);
		sprite02->Draw();
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
	Sprite::PostDraw();
}

void GameManager::TitleUpdate()
{
	//タイトル更新
	if (SceneNum == TITLE) {

		if (resetFlag == false) {
			//Initalize(dx12, audio, input);
			resetFlag = true;
		}

		if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::Button03)) {
			sprite01->Update();
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
	}
}
void GameManager::GameUpdate() {
	//ゲーム
	if (SceneNum == GAME) {

		//フェードアウト
		if (fade->GetFadeOut()) {
			fade->FadeOut();
		}

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
				//三体まで
				if (_enemy.size() <= 2) {
					Enemy* ene = nullptr;
					ene = Enemy::Appearance(model05, model01);
					if (ene != nullptr) {
						_enemy.push_back(ene);

						Sqhere _sqhere;
						_sqhere.radius = 5.0f;
						_sqhere.center = XMVectorSet(ene->position.x, ene->position.y, ene->position.z, 1);
						sqhere.push_back(_sqhere);
					}
				}
			}
			//エネミー関係の制御
			{
				for (int i = 0; i < _enemy.size(); i++) {
					_enemy[i]->moveUpdate(_player->model->position, cannon, obj03->position);
					_enemy[i]->Update();
					sqhere[i].center = XMVectorSet(_enemy[i]->position.x, _enemy[i]->position.y, _enemy[i]->position.z, 1);
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
							reception = 600;
						}

						//エネミーからのダメージ
						if (Hhit == true && _enemy[i]->attackHit == true) {
							_enemy[i]->attackHit = false;
							popHp += 10;
							_player->model->vmdNumber = vmdData::DAMAGE;
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
				XMVECTOR v = { (directInput->getLeftX()),0.0f,-(directInput->getLeftY()),0.0f };
				HitBox::hitBox[0]->position = XMFLOAT3(
					_player->model->position.x,
					_player->model->position.y + 10.0f,
					_player->model->position.z);

				if (_player->model->oldVmdNumber != vmdData::ATTACK) { _player->model->oldVmdNumber = _player->model->vmdNumber; }
				if (_player->model->oldVmdNumber != vmdData::DAMAGE) { _player->model->oldVmdNumber = _player->model->vmdNumber; }
				if (directInput->leftStickX() < 0.0f || directInput->leftStickX() > 0.0f || directInput->leftStickY() < 0.0f || directInput->leftStickY() > 0.0f) {
					_player->model->vmdNumber = vmdData::WALK;
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

					v = XMVector3TransformNormal(v, matRot);

					XMFLOAT3 _v(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);

					_player->model->SetMatRot(LookAtRotation(_v, XMFLOAT3(0.0f, 1.0f, 0.0f)));
				}
				else if (directInput->IsButtonPush(DirectInput::ButtonKind::Button01) || input->Push(DIK_X)) {
					_player->model->vmdNumber = vmdData::ATTACK;
					HitBox::hitBox[0]->scale = XMFLOAT3(10, 10, 10);
					HitBox::_hit[0].radius = 10;
					for (int i = 0; i < sqhere.size(); i++)
					{
						bool Hhit = Collision::CheckSqhere2Sqhere(sqhere[i], HitBox::_hit[0]);

						if (Hhit != true) { continue; }
						_enemy[i]->damage = true;
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
				//HP減少
				if (popHp != 0) {
					playerHp -= 1;
					popHp--;
				}

				//当たり判定確認
				CollisionManager::GetInstance()->CheckAllCollision();

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

		//更新処理(固有)
		{
			dx12->SceneUpdate();
			camera->Update();
			mainCamera->Update();
			stage->Update();
			obj03->Update();
			_player->Update();
			for (int i = 0; i < 6; i++) {
				cannon[i]->Update();
				cannon[i]->moveUpdate(_enemy);
			}
			HitBox::mainUpdate(_player->model->GetBoneMat(),_player->model->rotation);
			light->Update();

			hp->SetSize(XMFLOAT2(playerHp * 4.5f, 30));
			hp->Update();
		}
	}
}
void GameManager::EndUpdate() {
	//エンド
	if (SceneNum == END) {
		//エンド→タイトル遷移
		if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::Button01)) {
			SceneNum = TITLE;
			sprite01->Update();
		}
	}
}

XMFLOAT3 GameManager::moveCamera(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct)
{
	XMFLOAT3 pos;
	pos.x = pos1.x + ((pos2.x - pos1.x) * pct);
	pos.y = pos1.y + ((pos2.y - pos1.y) * pct);
	pos.z = pos1.z + ((pos2.z - pos1.z) * pct);
	return pos;
}
