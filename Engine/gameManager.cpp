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
	// nullptr�`�F�b�N
	assert(dx12);
	assert(input);
	assert(audio);

	this->dx12 = dx12;
	this->input = input;
	this->audio = audio;

	//this->audio->Load();// �f�o�b�O�e�L�X�g�p�e�N�X�`���ǂݍ���
	if (!Sprite::loadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
		return false;
	}
	debugText.Initialize(debugTextTexNumber);

	//�摜���\�[�X
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

	//�J�������Z�b�g
	camera = new DebugCamera(Application::window_width, Application::window_height, input);
	mainCamera = new Camera(Application::window_width, Application::window_height);
	setCamera = new Camera(Application::window_width, Application::window_height);
	camera->Update();

	Wrapper::SetCamera(mainCamera);
	dx12->SceneUpdate();

	//����
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
	//���C�g�Z�b�g
	Wrapper::SetLight(light);

	particlemanager = ParticleManager::Create();
	particlemanager->Update();

	Bottom = BillboardObject::Create(0);
	Bottom->Update();

	FbxObject3d::SetDevice(dx12->GetDevice());
	FbxObject3d::SetCamera(dx12->Camera());
	FbxObject3d::CreateGraphicsPipeline();

	//��{�I�u�W�F�N�g--------------
	defenceModel = Model::CreateFromOBJ("KSR-29");
	skyDomeModel = Model::CreateFromOBJ("skydome");
	bulletModel = Model::CreateFromOBJ("bullet");
	Box1x1 = Model::CreateFromOBJ("Box");

	golem = FbxLoader::GetInstance()->LoadModelFromFile("Golem");
	wolf = FbxLoader::GetInstance()->LoadModelFromFile("Wolf");

	//�X�e�[�W�f�[�^�y�у��f���f�[�^�ǂݍ���
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
	//��x���Z�b�g�i�x�[�X�L�����v�̓ǂݍ��݂̂��߁j
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

	//�h�q�C��
	for (int i = 0; i < 6; i++) {
		defense_facilities[i] = DefCannon::Appearance(0, defenceModel, defenceModel, defenceModel);
		defense_facilities[i]->BulletCreate(bulletModel);
		defense_facilities[i]->Update();
		defense_facilities[i]->scale = { 5,5,5 };
	}

	//�X�J�C�h�[��-------------------
	skyDome = Object3Ds::Create(skyDomeModel);
	skyDome->scale = { 7,7,7 };
	skyDome->position = { 0,350,0 };

	//MMD�I�u�W�F�N�g----------------
	modelPlayer = PMDmodel::CreateFromPMD("Resources/Model/�����~�N.pmd");
	modelPlayer->LoadVMDFile(vmdData::WAIT, "Resources/vmd/marieru_stand.vmd");
	modelPlayer->LoadVMDFile(vmdData::WALK, "Resources/vmd/Rick�����胂�[�V����02.vmd");
	modelPlayer->LoadVMDFile(vmdData::ATTACK, "Resources/vmd/attack.vmd");
	modelPlayer->LoadVMDFile(vmdData::DAMAGE, "Resources/vmd/�����_���[�W���[�V����.vmd");
	modelPlayer->LoadVMDFile(vmdData::AVOID, "Resources/vmd/Rick�����胂�[�V����05.vmd");

	//�v���C���[---------------------
	_player = Player::Create(modelPlayer);
	_player->SetInput(*input);
	_player->model->scale = { 1,1,1 };
	_player->model->position = { 548.0f,0,196.0f };
	_player->model->playAnimation();
	_player->model->animation = true;

	//�G�l�~�[-----------------------
	protEnemy[0] = Enemy::Create(wolf, golem);
	protEnemy[1] = Enemy::Create(wolf, golem);
	protEnemy[2] = Enemy::Create(wolf, golem);

	//�X�v���C�g---------------------
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

	//�V�[���G�t�F�N�g--------------------
	//�t�F�[�h�C���E�A�E�g
	fade = Fade::Create(9, { 0.0f,0.0f });
	fade->SetAlpha(0.0f);
	//�N���A��UI
	clear = Fade::Create(7, { Application::window_width / 2,Application::window_height / 2 });
	clear->SetAnchorPoint({ 0.5f,0.5f });
	clear->SetSize({ 480,480 });
	//�Q�[���I�[�o�[��UI
	failed = Fade::Create(8, { Application::window_width / 2,Application::window_height / 2 });
	failed->SetAnchorPoint({ 0.5f,0.5f });
	failed->SetSize({ 480,480 });
	//�X�^�[�g��UI
	start = Fade::Create(6, { 0,80 });
	start->SetAnchorPoint({ 0.5f,0.5f });
	start->SetAlpha(0.0f);
	start->SetSize({ 360,360 });
	start->Update();
	//��HP�i�ԏ�ԁj
	gateBreak_red = Fade::Create(12, { 72,134 });
	gateBreak_red->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_red->SetSize({ 80,80 });
	gateBreak_red->Update();
	//��HP�i����ԁj
	gateBreak_yellow = Fade::Create(13, { 72,134 });
	gateBreak_yellow->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_yellow->SetSize({ 80,80 });
	gateBreak_yellow->Update();
	//��HP�i�j�Ώ��
	gateBreak_green = Fade::Create(14, { 72,134 });
	gateBreak_green->SetAnchorPoint({ 0.5f,0.5f });
	gateBreak_green->SetSize({ 80,80 });
	gateBreak_green->Update();

	//�q�b�g�{�b�N�X-----------------
	HitBox::CreatePipeline(dx12);
	HitBox::CreateTransform();
	HitBox::CreateHitBox(_player->model->GetBonePos("����"), defenceModel);
	HitBox::hitBox[0]->scale = XMFLOAT3(5, 10, 5);
	triangle[0].p0 = XMVectorSet(stages[66]->position.x - 100.0f, stages[66]->position.y, stages[66]->position.z, 1);
	triangle[0].p1 = XMVectorSet(stages[66]->position.x - 100.0f, stages[66]->position.y + 120.0f, stages[66]->position.z, 1);
	triangle[0].p2 = XMVectorSet(stages[66]->position.x + 100.0f, stages[66]->position.y, stages[66]->position.z, 1);
	triangle[0].normal = XMVectorSet(0.0f, 0.0f, 1.0f, 0);

	//���͋y�щ���
	input->Update();
	audio->Load();

	SceneNum = TITLE;

	//�J�����̈ړ����̍쐬
	const float distanceFromPlayerToCamera = 10.0f;//�J�����ƃv���C���[�̋���������
	const float cameraHeight = 25.0f;//�J�����̍���
	vv0 = { -1.0f,0.0f,0.0f,0.0f };//�J�����̐��ʃx�N�g��������
	XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

	afterEye.x = _player->model->position.x + direction.x * distanceFromPlayerToCamera;
	afterEye.y = _player->model->position.y + direction.y * distanceFromPlayerToCamera;
	afterEye.z = _player->model->position.z + direction.z * distanceFromPlayerToCamera;
	afterEye.y += cameraHeight;
	mainCamera->SetEye(afterEye);
	//�J���������_������
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
		//imgui��UI�R���g���[��
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

		//�J�����؂�ւ�
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

	//���C�g
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
	//�^�C�g���X�V
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
		//�t�F�[�h�C��
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
	//�Q�[��
	if (SceneNum == GAME) {

		//�t�F�[�h�A�E�g
		if (fade->GetFadeOut()) {
			fade->FadeOut();
		}

		//�����蔻��m�F
		CollisionManager::GetInstance()->CheckAllCollision();

		//�Q�[���X�^�[�g������
		if (GameModeNum == GameMode::START) {

			//�J�����̈ړ���̍쐬
			static float step = 0;
			const float distanceFromPlayerToCamera = 40.0f;//�J�����ƃv���C���[�̋���������
			const float cameraHeight = 25.0f;//�J�����̍���
			vv0 = { 0.0f,0.0f,1.0f,0.0f };//�J�����̐��ʃx�N�g��������
			XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

			XMFLOAT3 beforeEye;
			beforeEye.x = _player->model->position.x + direction.x * distanceFromPlayerToCamera;
			beforeEye.y = _player->model->position.y + direction.y * distanceFromPlayerToCamera;
			beforeEye.z = _player->model->position.z + direction.z * distanceFromPlayerToCamera;
			beforeEye.y += cameraHeight;

			//�J���������_������
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

			//�X�^�[�g���摜�\��
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
		//�Q�[���i�s������
		if (GameModeNum == GameMode::NASI) {
			//�G�l�~�[�̐���
			{
				//�������ԂɂȂ�A�����Ώۂ������Ă��Ȃ��Ȃ�
				if (enemyPopTime >= 1.0f && protEnemy[enemyNum]->alive == false) {

					protEnemy[enemyNum]->Appearance();
					//���퐶��
					//protEnemy[enemyNum]->CreateWeapon(Model::CreateFromOBJ("weapon"));
					//�p�[�e�B�N������
					protEnemy[enemyNum]->CreateParticle();

					_enemy.push_back(protEnemy[enemyNum]);

					//�����蔻��p���̐���
					Sqhere _sqhere;
					_sqhere.radius = 5.0f;
					_sqhere.center = XMVectorSet(_enemy[enemyNum]->GetPosition().x, _enemy[enemyNum]->GetPosition().y, _enemy[enemyNum]->GetPosition().z, 1);
					sqhere.push_back(_sqhere);

					enemyNum++;
					if (enemyNum >= 3) { enemyNum = 0; }
				}
				enemyPopTime += 1.0f / 60.0f;
				//�O�̂܂�
				//if (_enemy.size() <= 2) {
				//	if (useModel >= 3) { useModel = 0; }
					//ene = Enemy::Appearance(golem[useModel], wolf[useModel]);
					//if (ene != nullptr) {
					//	//�i�[
					//	_enemy.push_back(ene);
					//	useModel += 1;
				//	}
				//}
			}
			//�G�l�~�[�֌W�̐���
			{
				for (int i = 0; i < _enemy.size(); i++) {
					_enemy[i]->moveUpdate(_player->model->position, defense_facilities, stages[66]->position);
					sqhere[i].center = XMVectorSet(_enemy[i]->GetPosition().x, _enemy[i]->GetPosition().y, _enemy[i]->GetPosition().z, 1);
					if (_enemy[i]->alive == true) { continue; }
					_enemy.erase(_enemy.begin());
					sqhere.erase(sqhere.begin());
				}
			}
			//�����蔻��i�v���C���[ / �G / �ŏI�֖�j
			{
				for (int i = 0; i < sqhere.size(); i++) {
					for (int j = 0; j < HitBox::_hit.size(); j++) {
						bool Hhit = Collision::CheckSqhere2Sqhere(sqhere[i], HitBox::_hit[j]);
						XMVECTOR inter;
						bool Ghit = Collision::CheckSqhere2Triangle(sqhere[i], triangle[0], &inter);

						//�Q�[�g�U��
						if (Ghit == true && reception <= 0 && _enemy[i]->attackHit == true) {
							if (_enemy[i]->mode != 3) { continue; }
							Hhit = false;
							_enemy[i]->attackHit = false;
							gateHP -= 1;
							shake = true;
							reception = 600;
						}

						//�G�l�~�[����̃_���[�W
						if (Hhit == true && _enemy[i]->attackHit == true) {
							_enemy[i]->attackHit = false;
							popHp += 10;
							//_player->model->vmdNumber = vmdData::DAMAGE;
						}
						//�Q�[���I�[�o�[����				
						//if (gateHP <= 0 || playerHp <= 0) { SceneChange = true; }
						reception--;
					}
				}
			}

			//�ړ�
			{
				//�ړ��x�N�g��
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
					//���ړ�
					if (input->Push(DIK_A) || directInput->leftStickX() < 0.0f) {
						_player->model->position = (MoveLeft(_player->model->position));
						for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveLeft(HitBox::GetHit()[i]->position); }
					}
					//�E�ړ�
					if (input->Push(DIK_D) || directInput->leftStickX() > 0.0f) {
						_player->model->position = (MoveRight(_player->model->position));
						for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveRight(HitBox::GetHit()[i]->position); }
					}
					//���ړ�
					if (input->Push(DIK_W) || directInput->leftStickY() < 0.0f) {
						_player->model->position = (MoveBefore(_player->model->position));
						for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveBefore(HitBox::GetHit()[i]->position); }
					}
					//��ړ�
					if (input->Push(DIK_S) || directInput->leftStickY() > 0.0f) {
						_player->model->position = (MoveAfter(_player->model->position));
						for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveAfter(HitBox::GetHit()[i]->position); }
					}
					XMMATRIX matRot = XMMatrixIdentity();
					//�p�x��]
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
						//�̗͂��Ȃ��Ȃ��Ă����
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
			//�J�������[�N(�v���C���[)
			float angleH = 100.0f;
			float angleV = 40.0f;
			if (directInput->rightStickX() >= 0.5f || directInput->rightStickX() <= -0.5f) {
				angleHorizonal +=
					XMConvertToRadians(angleH * directInput->getRightX());
			}
			if (directInput->rightStickY() >= 0.5f || directInput->rightStickY() <= -0.5f) {
				angleVertical +=
					XMConvertToRadians(angleV * directInput->getRightY());
				//�����p�x
				if (angleVertical >= 20) {
					angleVertical = 20;
				}
				//�����p�x
				if (angleVertical <= -20) {
					angleVertical = -20;
				}
			}
			if (input->Push(DIK_RIGHT) || input->Push(DIK_LEFT) || input->Push(DIK_UP) || input->Push(DIK_DOWN)) {
				//�E
				if (input->Push(DIK_RIGHT)) {
					angleHorizonal +=
						XMConvertToRadians(angleH * directInput->getRightX());
				}
				//��
				if (input->Push(DIK_LEFT)) {
					angleHorizonal -=
						XMConvertToRadians(angleH);
				}
				//��
				if (input->Push(DIK_UP)) {
					angleVertical -=
						XMConvertToRadians(angleV);
					//�����p�x
					if (angleVertical <= -20) {
						angleVertical = -20;
					}
				}
				//��
				if (input->Push(DIK_DOWN)) {
					angleVertical +=
						XMConvertToRadians(angleV);
					//�����p�x
					if (angleVertical >= 20) {
						angleVertical = 20;
					}
				}
			}
			//�J�������[�N(�Ǐ])
			{
				//�J�����ƃv���C���[�̋���������
				const float distanceFromPlayerToCamera = 40.0f;

				//�J�����̍���
				const float cameraHeight = 25.0f;

				//�J�����̐��ʃx�N�g��������
				vv0 = { 0.0f,0.0f,1.0f,0.0f };

				//�c���̉�]
				rotM = XMMatrixRotationX(XMConvertToRadians(angleVertical));
				vv0 = XMVector3TransformNormal(vv0, rotM);

				//�����̉�]
				rotM = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));
				vv0 = XMVector3TransformNormal(vv0, rotM);

				//
				XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

				//�J�����ʒu������
				XMFLOAT3 _eye;
				_eye.x = _player->model->position.x + direction.x * distanceFromPlayerToCamera;
				_eye.y = _player->model->position.y + direction.y * distanceFromPlayerToCamera;
				_eye.z = _player->model->position.z + direction.z * distanceFromPlayerToCamera;
				_eye.y += cameraHeight;
				mainCamera->SetEye(_eye);

				//�J���������_������
				XMFLOAT3 _target;
				_target.x = _player->model->position.x - direction.x * distanceFromPlayerToCamera;
				_target.y = _player->model->position.y - direction.y * distanceFromPlayerToCamera;
				_target.z = _player->model->position.z - direction.z * distanceFromPlayerToCamera;
				_target.y += cameraHeight / 1.5f;
				mainCamera->SetTarget(_target);

				mainCamera->Update();
			}
			//�X�V����(�Q�[��)
			{
				//�h�q�{�ݑ���
				for (int i = 0; i < 6; i++) {
					if (distance(_player->model->position, defense_facilities[i]->position) >= 3) { continue; }
					if (!(input->Trigger(DIK_F) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonB))) { continue; }
					setObjectPos = defense_facilities[i]->position;
					SetNum = i;
					//�J���������_������
					//�J�����ƃv���C���[�̋���������
					const float distanceFromPlayerToCamera = 40.0f;
					//�J�����̍���
					const float cameraHeight = 10.0f;

					//�J�����̐��ʃx�N�g��������
					vv0 = { 0.0f,0.0f,1.0f,0.0f };
					//
					XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

					//�J�����ʒu������
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

				//HP����
				if (popHp != 0) {
					playerHp -= 1;
					popHp--;
				}

				if (input->Trigger(DIK_SPACE)) {
					gateBreak_red->SetShake(true);
					gateBreak_yellow->SetShake(true);
					gateBreak_green->SetShake(true);
				}

				//�Q�[�g�_���[�W�����A�N�V����
				if (shake) {
					Shake3D(stages[66]->position);
				}

				//�X�e�[�W�ړ�
				if (UseStage == GameLocation::BaseCamp && _player->model->position.z <= 5.0f) {
					UseStage = GameLocation::BaseStage;
					_player->model->SetPosition(XMFLOAT3(225.0f, 0.0f, 275.0f));
					angleHorizonal = 90.0f;
				}

				//�{�ݐݒu
				if (directInput->IsButtonPush(DirectInput::ButtonKind::ButtonB)) {
					for (int i = 0; i < stages.size(); i++) {
						//�y��łȂ���΃X���[
						if (stages[i]->GetObjectNum() != ObjectType::FounDation) { continue; }

						//�v���C���[�Ɠy��̋������v�Z
						if (distance(stages[i]->position, _player->model->position) > 5) { continue; }

						//�{�݂��ݒu����Ă��Ȃ���΃X���[
						if (stages[i]->GetInstallation()) { continue; }

						//�{�ݔԍ���ۑ�
						UseFoundation = i;

						//���{�^������t���[��������p�t���O
						WeaponSelectDo = true;

						//�{�ݐݒu�Ɉڍs
						GameModeNum = GameMode::WEAPONSELECT;
					}
				}

				//�ꎞ��~
				if (directInput->IsButtonPush(directInput->Button09) || input->Trigger(DIK_ESCAPE)) {
					GameModeNum = GameMode::POSE;
					pose = true;
				}

				if (input->Push(DIK_Q)) {
					GameModeNum = GameMode::OVER;
					fade->SethalfFade(true);
				}
				//�Q�[���N���A����
				if (repelCount >= 15) {
					GameModeNum = GameMode::CLEAR;
					fade->SethalfFade(true);
				}
				//�Q�[���I�[�o�[����
				if (playerHp <= 0 || gateHP <= 0) {
					GameModeNum = GameMode::OVER;
					fade->SethalfFade(true);
				}

				if (SceneChange) {
					SceneNum = END;
				}
			}

			//�p�[�e�B�N������
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

			//�{�ݑI��
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

			//�ݒu�{�݌���
			if (directInput->IsButtonPush(DirectInput::ButtonB) && !WeaponSelectDo) {
				//�y��Ɏ{�݂�ݒu
				defense_facilities[WeaponCount]->SetPosition({
					stages[UseFoundation]->position.x,
					10,stages[UseFoundation]->position.z });
				defense_facilities[WeaponCount]->SetAlive(true);
				//�{�ݔԍ�������
				WeaponCount += 1;

				//�ݒu��ԗL��
				stages[UseFoundation]->SetInstallation(true);

				//�Q�[���ɖ߂�
				GameModeNum = GameMode::NASI;
			}

			if (directInput->IsButtonPush(DirectInput::ButtonA)) {
				GameModeNum = GameMode::NASI;
			}
			WeaponSelectDo = false;
		}
		//�h�q�{�ݑ��쎞����
		if (GameModeNum == GameMode::SET) {
			float angleH = 50.0f;
			float angleV = 5.0f;
			XMMATRIX matRot = XMMatrixIdentity();
			//�J�����̐��ʃx�N�g��������
			vv0 = { 0.0f,0.0f,1.0f,0.0f };

			if (directInput->leftStickX() >= 0.5f || directInput->leftStickX() <= -0.5f) {
				angleHorizonal +=
					XMConvertToRadians(angleH * directInput->getLeftX());
			}

			//�p�x��]
			matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));

			vv0 = XMVector3TransformNormal(vv0, matRot);
			XMFLOAT3 set_v;
			set_v.x = vv0.m128_f32[0];
			set_v.y = vv0.m128_f32[1];
			set_v.z = vv0.m128_f32[2];

			defense_facilities[SetNum]->SetMatRot(LookAtRotation(set_v, XMFLOAT3(0.0f, 1.0f, 0.0f)));
			defense_facilities[SetNum]->SetShotVec(vv0);
			//�J���������_������
			//�J�����ƃv���C���[�̋���������
			const float distanceFromPlayerToCamera = 40.0f;
			//�J�����̍���
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
		//�Q�[���N���A������
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
		//�Q�[���I�[�o�[������
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
		//�|�[�Y��ʎ�����
		if (GameModeNum == GameMode::POSE) {
			if (!pose) {
				if (directInput->IsButtonUp(directInput->ButtonPouse) || input->Trigger(DIK_ESCAPE)) {
					GameModeNum = GameMode::NASI;
				}
			}
			pose = false;
		}

		//�X�V����(�ŗL)
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

		//�X�e�[�W�`��X�V����
		for (auto& object : stages) {
			object->Update();
		}
		for (auto& object : baseCamp) {
			object->Update();
		}
	}
}
void GameManager::EndUpdate() {
	//�G���h
	if (SceneNum == END) {
		//�G���h���^�C�g���J��
		if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			SceneNum = TITLE;
			Title->Update();
		}
	}
}

void GameManager::Draw()
{
	// �R�}���h���X�g�̎擾
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

		//�[�x�o�b�t�@�N���A
		dx12->ClearDepthBuffer();

		Bottom->Draw(cmdList);

		BaseObject::PreDraw(cmdList);

		//�v���C���[�`��
		if (GameModeNum != GameMode::SET) {
			_player->Draw();
		}
		skyDome->Draw();

		if (UseStage == GameLocation::BaseCamp) {
			//�x�[�X�L�����v�`��
			for (auto& object : baseCamp) {
				object->Draw();
			}
		}
		//�X�e�[�W�`��
		else if (UseStage == GameLocation::BaseStage) {
			for (auto& object : stages) {
				object->Draw();
			}
			//�h�q�{�ݕ`��
			for (int i = 0; i < 6; i++) {
				if (!defense_facilities[i]->GetAlive()) { continue; }
				defense_facilities[i]->Draw();
			}

			//�G�`��
			for (int i = 0; i < _enemy.size(); i++) {
				_enemy[i]->Draw(cmdList);
			}
		}

		BaseObject::PostDraw();

		// 3D�I�u�W�F�N�g�`��O����
		ParticleManager::PreDraw(cmdList);
		// 3D�I�u�N�W�F�N�g�̕`��
		particlemanager->Draw();
		// 3D�I�u�W�F�N�g�`��㏈��
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

	//�t�F�[�h�p�摜�`��
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
