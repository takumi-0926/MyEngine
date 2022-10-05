#include <algorithm>
#include "gameManager.h"
#include "..\object\baseObject.h"
#include "..\FBX\FbxLoader.h"
#include "..\FBX\FbxObject3d.h"
#include "../PMD/PMXLoader.h"

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

	//this->audio->Load();
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

	//�J�������Z�b�g
	camera	   = new DebugCamera(Application::window_width, Application::window_height, input);
	mainCamera = new Camera(Application::window_width, Application::window_height);
	camera->SetTarget({ 0,20,0 });
	camera->SetDistance(20.0f);
	camera->Update();
	Wrapper::SetCamera(mainCamera);
	dx12->SceneUpdate();

	//���C�g�Z�b�g
	light = light->Create();
	light->SetLightColor({ 1,1,1 });
	Wrapper::SetLight(light);

	//��{�I�u�W�F�N�g--------------
	model01 = Model::CreateFromOBJ("Block");
	model02 = Model::CreateFromOBJ("ground");
	model03 = Model::CreateFromOBJ("Cannon");
	model04 = Model::CreateFromOBJ("Gate");
	model05 = Model::CreateFromOBJ("sqhere");
	model06 = Model::CreateFromOBJ("maru");

	//�S�̃X�e�[�W
	stage = Stage::Create();
	stage->SetModel(model02);
	stage->Update();
	stage->scale = { 2,2,2 };
	stage->rotation.y = 90;
	stage->SetPosition({ 0,-30,0 });

	//�Q�[�g�i�ŏI�֖�j
	obj03 = Object3Ds::Create();
	obj03->SetModel(model04);
	obj03->Update();
	obj03->scale = { 50,50,50 };
	obj03->SetPosition({ 0,0,150 });

	//�h�q�C��
	for (int i = 0; i < 6; i++)
	{
		cannon[i] = DefCannon::Create();
		cannon[i]->SetModel(model03);
		cannon[i]->BulletCreate(model06);
		cannon[i]->Update();
		cannon[i]->scale = { 10,10,10 };
	}
	cannon[0]->SetPosition({ -50,15,  0 });
	cannon[1]->SetPosition({ 50,15,  0 });
	cannon[2]->SetPosition({ 50,15, 50 });
	cannon[3]->SetPosition({ -50,15, 50 });
	cannon[4]->SetPosition({ 50,15,100 });
	cannon[5]->SetPosition({ -50,15,100 });

	//MMD�I�u�W�F�N�g----------------
	pmdObject.reset(new PMDobject(dx12));
	pmdModel.reset(new PMDmodel(dx12, "Resources/Model/�����~�Nmetal.pmd", *pmdObject));
	pmdModel->scale = { 1,1,1 };
	pmdModel->SetPosition({ 0,10,0 });

	//�X�v���C�g---------------------
	sprite01 = Sprite::Create(0, { 0.0f,0.0f });
	sprite02 = Sprite::Create(1, { 0.0f,0.0f });
	sprite03 = Sprite::Create(2, { 0.0f,0.0f });
	sprite04 = Sprite::Create(3, { 0.0f,0.0f });

	for (int i = 0; i < P_HP; i++) {
		static float xpos = 4.0f;
		hp[i] = Sprite::Create(4, { xpos * i + 36.0f,32.0f });
		hp[i]->SetSize(XMFLOAT2(30, 30));
		hp[i]->Update();
	}

	Damege = Sprite::Create(5, { 0,0 });
	Damege->Update();

	//�q�b�g�{�b�N�X-----------------
	HitBox::CreatePipeline(dx12);
	HitBox::CreateTransform();
	HitBox::CreateHitBox(pmdModel->GetBonePos("����"), model01);
	HitBox::hitBox[0]->scale = XMFLOAT3(5, 10, 5);
	triangle[0].p0 = XMVectorSet(obj03->position.x - 100.0, obj03->position.y, obj03->position.z, 1);
	triangle[0].p1 = XMVectorSet(obj03->position.x - 100.0, obj03->position.y + 120.0, obj03->position.z, 1);
	triangle[0].p2 = XMVectorSet(obj03->position.x + 100.0, obj03->position.y, obj03->position.z, 1);
	triangle[0].normal = XMVectorSet(0.0f, 0.0f, 1.0f, 0);

	//���͋y�щ���
	input->Update();
	audio->Load();

	SceneNum = TITLE;
	move1.flag = false;
	move2.flag = false;

	pmdModel->playAnimation();
	pmdModel->animation = true;

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
		//imgui��UI�R���g���[��
		ImGui::Text("test %.2f ,%.2f ,%.2f ,%.2f", a, b, c, d);
		ImGui::Checkbox("EnemyPop", &blnChk);
		ImGui::Checkbox("test", &pmdModel->a);
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

		//�J�����؂�ւ�
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

	//���C�g
	static XMVECTOR lightDir = { 0,1,5,0 };
	{
		if (input->Push(DIK_T)) { lightDir.m128_f32[1] += 1.0f; }
		if (input->Push(DIK_G)) { lightDir.m128_f32[1] -= 1.0f; }
		if (input->Push(DIK_H)) { lightDir.m128_f32[0] += 1.0f; }
		if (input->Push(DIK_F)) { lightDir.m128_f32[0] -= 1.0f; }

		light->SetLightDir(lightDir);
	}

	//�^�C�g��
	if (SceneNum == TITLE) {

		if (resetFlag == false) {
			//Initalize(dx12, audio, input);
			resetFlag = true;
		}
		if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::Button03)) {
			SceneNum = GAME;
			sprite01->Update();
		}
	}
	//�Q�[��
	else if (SceneNum == GAME) {
		//�G�l�~�[�̐���
		{
			static float popTime = 0;
			if (popTime >= 10.0f) {
				Enemy* ene = Enemy::Create();
				int r = rand() % 10;
				if (r % 2 == 1) {
					ene->SetModel(model05);
					ene->mode = 2;
				}
				else if (r % 2 != 1) {
					ene->SetModel(model06);
					ene->mode = 3;
				}
				ene->scale = { 10,10,10 };
				ene->step = 0.00005;
				ene->alive = true;
				_enemy.push_back(ene);

				Sqhere _sqhere;
				_sqhere.radius = 5.0f;
				_sqhere.center = XMVectorSet(ene->position.x, ene->position.y, ene->position.z, 1);
				sqhere.push_back(_sqhere);

				popTime = 0;
			}
			else {
				popTime += 1.0f / 60.0f;
			}
		}
		//�G�l�~�[�֌W�̐���
		for (int i = 0; i < _enemy.size(); i++) {
			_enemy[i]->moveUpdate(pmdModel->position, cannon, obj03->position);
			_enemy[i]->rotation.y += 1.0f;
			sqhere[i].center = XMVectorSet(_enemy[i]->position.x, _enemy[i]->position.y, _enemy[i]->position.z, 1);
			if (_enemy[i]->alive == true) { continue; }
			_enemy.erase(_enemy.begin());
			sqhere.erase(sqhere.begin());
		}
		//�����蔻��i�v���C���[ / �G / �ŏI�֖�j
		for (int i = 0; i < sqhere.size(); i++) {
			for (int j = 0; j < HitBox::_hit.size(); j++) {
				bool Hhit = Coliision::CheckSqhere2Sqhere(sqhere[i], HitBox::_hit[j]);
				XMVECTOR inter;
				bool Ghit = Coliision::CheckSqhere2Triangle(sqhere[i], triangle[0], &inter);

				//�Q�[�g�U��
				if (Ghit == true && reception <= 0 && _enemy[i]->attackHit == true) {
					if (_enemy[i]->mode != 3) { continue; }
					Hhit = false;
					_enemy[i]->attackHit = false;
					gateHP -= 1;
					reception = 600;
				}

				//�G�l�~�[����̃_���[�W
				if (Hhit == true && _enemy[i]->attackHit == true) {
					if (_enemy[i]->mode != 2) { continue; }
					_enemy[i]->attackHit = false;
					enemyToPlayerDamege = true;
					DamegeAlpha = 1.0f;
					playerHp -= 10;
				}
				//�Q�[���I�[�o�[����				
				if (gateHP <= 0 || playerHp <= 0) { SceneNum = END; }
				reception--;
			}
		}
		//�����蔻��(�v���C���[/�X�e�[�W)��
		if (pmdModel->position.y < stage->position.y + 40) {
			pmdModel->position.y = stage->position.y + 40;
		}
		//�G�l�~�[���m�̓����蔻��
		//for (int i = 0; i < sqhere.size(); i++) {
		//	for (int j = 0; j < sqhere.size(); j++) {
		//		if (i == j) { continue; }
		//		bool Hhit = Coliision::CheckSqhere2Sqhere(sqhere[i], sqhere[j]);
		//		if (Hhit == true) {
		//			_enemy[i]->position = _enemy[i]->oldPos;
		//		}
		//	}
		//}

		//�ړ�
		{
			//�ړ��x�N�g��
			XMVECTOR v = { (directInput->getLeftX()),0.0f,-(directInput->getLeftY()),0.0f };
			HitBox::hitBox[0]->position = XMFLOAT3(
				pmdModel->position.x,
				pmdModel->position.y + 10.0f,
				pmdModel->position.z);

			if (pmdModel->oldVmdNumber != vmdData::ATTACK) { pmdModel->oldVmdNumber = pmdModel->vmdNumber; }
			if (directInput->leftStickX() < 0.0f || directInput->leftStickX() > 0.0f || directInput->leftStickY() < 0.0f || directInput->leftStickY() > 0.0f) {
				pmdModel->vmdNumber = vmdData::WALK;
				HitBox::hitBox[0]->scale = XMFLOAT3(5, 10, 5);
				HitBox::_hit[0].radius = 5;

				if (directInput->getTriggerZ() != 0) {
					speed = 2.0f;
				}
				else { speed = 1.0f; }
				//���ړ�
				if (input->Push(DIK_A) || directInput->leftStickX() < 0.0f) {
					pmdModel->position = MoveLeft(pmdModel->position);
					for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveLeft(HitBox::GetHit()[i]->position); }
				}
				//�E�ړ�
				if (input->Push(DIK_D) || directInput->leftStickX() > 0.0f) {
					pmdModel->position = MoveRight(pmdModel->position);
					for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveRight(HitBox::GetHit()[i]->position); }
				}
				//���ړ�
				if (input->Push(DIK_W) || directInput->leftStickY() < 0.0f) {
					pmdModel->position = MoveBefore(pmdModel->position);
					for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveBefore(HitBox::GetHit()[i]->position); }
				}
				//��ړ�
				if (input->Push(DIK_S) || directInput->leftStickY() > 0.0f) {
					pmdModel->position = MoveAfter(pmdModel->position);
					for (int i = 0; i < HitBox::GetHit().size(); i++) { HitBox::GetHit()[i]->position = MoveAfter(HitBox::GetHit()[i]->position); }
				}
				XMMATRIX matRot = XMMatrixIdentity();
				//�p�x��]
				matRot = XMMatrixRotationY(XMConvertToRadians(angleHorizonal));
				v = XMVector3TransformNormal(v, matRot);
				XMFLOAT3 _v(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
				pmdModel->SetMatRot(LookAtRotation(_v, XMFLOAT3(0.0f, 1.0f, 0.0f)));
			}
			else if (directInput->IsButtonPush(DirectInput::ButtonKind::Button01) || input->Push(DIK_X)) {
				pmdModel->vmdNumber = vmdData::ATTACK;
				HitBox::hitBox[0]->scale = XMFLOAT3(10, 10, 10);
				HitBox::_hit[0].radius = 10;
				for (int i = 0; i < sqhere.size(); i++)
				{
					bool Hhit = Coliision::CheckSqhere2Sqhere(sqhere[i], HitBox::_hit[0]);

					if (Hhit != true) { continue; }
					_enemy[i]->damage = true;
					_enemy[i]->status.HP -= 1;
					//�̗͂��Ȃ��Ȃ��Ă����
					if (_enemy[i]->status.HP <= 0) {
						_enemy[i]->alive = false;
					}
				}
			}
			else {
				pmdModel->vmdNumber = vmdData::WAIT;
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
			const float cameraHeight = 30.0f;

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
			_eye.x = pmdModel->position.x + direction.x * distanceFromPlayerToCamera;
			_eye.y = pmdModel->position.y + direction.y * distanceFromPlayerToCamera;
			_eye.z = pmdModel->position.z + direction.z * distanceFromPlayerToCamera;
			_eye.y += cameraHeight;
			mainCamera->SetEye(_eye);

			//�J���������_������
			XMFLOAT3 _target;
			_target.x = pmdModel->position.x - direction.x * distanceFromPlayerToCamera;
			_target.y = pmdModel->position.y - direction.y * distanceFromPlayerToCamera;
			_target.z = pmdModel->position.z - direction.z * distanceFromPlayerToCamera;
			_target.y += cameraHeight / 3;
			mainCamera->SetTarget(_target);

			mainCamera->Update();
		}
		//�X�V����(�Q�[��)
		{
			static float gravity = 0.098f;
			static float v = 0.0f;
			v += gravity;
			pmdModel->position.y -= gravity;

			static float count = 0.0f;
			if (enemyToPlayerDamege == true) {
				count += 1.0f / 60.0f;
				if (count >= 1.5f) {
					DamegeAlpha -= 1.0f / 120.0f;
					Damege->SetAlpha(DamegeAlpha);
					if (DamegeAlpha <= 0) {
						enemyToPlayerDamege = false;
						count = 0;
					}
				}
			}
		}
		//�X�V����(�ŗL)
		{
			dx12->SceneUpdate();
			camera->Update();
			stage->Update();
			obj03->Update();
			pmdModel->Update();
			for (int i = 0; i < 6; i++) {
				cannon[i]->Update();
				cannon[i]->moveUpdate(_enemy);
			}
			HitBox::mainUpdate(pmdModel->GetBoneMat(), pmdModel->rotation);
			light->Update();

			Damege->Update();
		}

	}
	//�G���h
	else if (SceneNum == END) {
		//�G���h���^�C�g���J��
		if (input->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::Button01)) {
			SceneNum = TITLE;
			sprite01->Update();
		}
	}
}

void GameManager::Draw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	if (SceneNum == TITLE) {
		Sprite::PreDraw(cmdList);
		sprite01->Draw();
		Sprite::PostDraw();
	}
	else if (SceneNum == GAME) {
		Sprite::PreDraw(cmdList);
		Sprite::PostDraw();

		//�[�x�o�b�t�@�N���A
		dx12->ClearDepthBuffer();

		BaseObject::PreDraw(cmdList);
		stage->Draw();
		obj03->Draw();
		for (int i = 0; i < 6; i++) {
			cannon[i]->Draw();
		}
		for (int i = 0; i < _enemy.size(); i++) {
			if (_enemy[i]->damage == true) {
				_enemy[i]->damegeCount += 1.0f / 60.0f;
				if (_enemy[i]->damegeCount >= 0.1f) {
					_enemy[i]->damage = false;
					_enemy[i]->damegeCount = 0;
				}
				continue;
			}

			_enemy[i]->Draw();

		}
		pmdObject->Draw();
		dx12->SceneDraw();
		pmdModel->Draw(cmdList);

		BaseObject::PostDraw();

		Sprite::PreDraw(cmdList);
		for (int i = 0; i < playerHp; i++) {
			hp[i]->Draw();
		}

		sprite04->Draw();
		if (enemyToPlayerDamege == true) {
			Damege->Draw();
		}

		Sprite::PostDraw();
	}
	else if (SceneNum == END) {
		Sprite::PreDraw(cmdList);
		sprite02->Draw();
		Sprite::PostDraw();
	}
}
