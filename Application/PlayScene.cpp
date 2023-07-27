#include "PlayScene.h"
#include "Player.h"
#include "Camera/DebugCamera.h"

#include "UIManager.h"
#include "EnemyManager.h"
#include "StageManager.h"
#include "Math/MyMath.h"
#include "OBBCollision.h"

#include <future>
#include <Singleton_Heap.h>

static const int debugTextTexNumber = 99;
std::thread th = {};

void PlayScene::Initialize(Wrapper* _dx12)
{
	assert(_dx12);

	this->dx12 = _dx12;

	StageManager::GetInstance()->SetUseStage(1);

	//��{�I�u�W�F�N�g--------------
	defenceModel = Model::CreateFromOBJ("KSR-29");
	bulletModel = Model::CreateFromOBJ("bullet");

	//�J�������Z�b�g
	mainCamera = new Camera(Application::window_width, Application::window_height);
	setCamera = new Camera(Application::window_width, Application::window_height);

	Wrapper::SetCamera(mainCamera);
	FbxObject3d::SetCamera(Wrapper::GetCamera());
	Object3Ds::SetCamera(Wrapper::GetCamera());

	//����
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
	//���C�g�Z�b�g
	Wrapper::SetLight(light);

	particlemanager = ParticleManager::Create();
	particlemanager->Update();
}

void PlayScene::Finalize()
{
}

void PlayScene::Update()
{
	UIManager* UI = UIManager::GetInstance();

	UI->GameUpdate();

	StageManager* stage = StageManager::GetInstance();

	stage->Update();

	mainCamera->Update();
	setCamera->Update();

	//���C�g�Z�b�g
	Wrapper::SetLight(light);

	//���C�g
	light->SetPointLightPos(0,
		XMFLOAT3(StageManager::GetInstance()->GetBaseCamp(29)->position.x,
			StageManager::GetInstance()->GetBaseCamp(29)->position.y + 5.0f,
			StageManager::GetInstance()->GetBaseCamp(29)->position.z));

	//���[�h
	if (!load && GameModeNum == NONE) {
		Wrapper::SetCamera(mainCamera);
		load = true;
	}

	if (load) {
		loading();

		if (!load) {
			GameModeNum = START;
			UI->SetMode(START);
		}

		if (!loadComplate) { return; }
	}

	//�t�F�[�h�A�E�g
	if (UI->GetFade()->GetFadeOut() && !load) {
		UI->GetFade()->FadeOut();
	}
	//�Q�[���X�^�[�g������
	if (GameModeNum == GameMode::START) {

		_player->SetPosition(XMFLOAT3(0.0f, 0.0f, 100.0f));
		_player->Update();

		//�J�����̈ړ���̍쐬
		static float step = 0;
		const float distanceFromPlayerToCamera = 50.0f;//�J�����ƃv���C���[�̋���������
		const float cameraHeight = 30.0f;//�J�����̍���
		vv0 = { 0.0f,0.0f,1.0f,0.0f };//�J�����̐��ʃx�N�g��������
		XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

		XMFLOAT3 beforeEye;
		beforeEye = _player->GetPosition();
		beforeEye += direction * distanceFromPlayerToCamera;

		beforeEye.y += cameraHeight * 1.3f;

		//�J���������_������
		XMFLOAT3 _target;
		_target = _player->GetPosition();
		_target.y += cameraHeight;
		mainCamera->SetTarget(_target);
		mainCamera->SetEye(moveCamera(mainCamera->GetEye(), beforeEye, step += 0.0005f));
		XMFLOAT3 e = mainCamera->GetEye();
		if (samePoint(e, beforeEye)) {
			GameModeNum = GameMode::PLAY;
			UI->SetMode(PLAY);
			step = 0.0f;
		}

		//�X�^�[�g���摜�\��
		UI->GetStarted()->SlideIn();
		UI->GetStarted()->FadeIn();
		UI->GetStarted()->SlideOut();
		if (UI->GetStarted()->GetSlideOut()) {
			UI->GetStarted()->FadeOut();
			if (!UI->GetStarted()->GetFadeOut()) {
				GameModeNum = GameMode::PLAY;
				UI->SetMode(PLAY);
			}
		}
		UI->GetStarted()->Update();
	}
	//�Q�[���i�s�O����
	else if (GameModeNum == GameMode::Preparation) {

		_player->Update();

		//�J�������[�N(�Ǐ])
		{
			//�J�����ƃv���C���[�̋���������
			const float distanceFromPlayerToCamera = 50.0f;

			//�J�����̍���
			const float cameraHeight = 30.0f;

			XMMATRIX rotM = {};
			XMVECTOR vv0 = {};

			//�J�����̐��ʃx�N�g��������
			vv0 = { 0.0f,0.0f,1.0f,0.0f };
			//�c���̉�]
			rotM = XMMatrixRotationX(XMConvertToRadians(_player->GetAngleVertical()));
			vv0 = XMVector3TransformNormal(vv0, rotM);

			//�����̉�]
			rotM = XMMatrixRotationY(XMConvertToRadians(_player->GetAngleHorizonal()));
			vv0 = XMVector3TransformNormal(vv0, rotM);

			//
			XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

			//�J�����ʒu������
			XMFLOAT3 _eye;
			_eye = _player->GetPosition();
			_eye += direction * distanceFromPlayerToCamera;
			_eye.y += cameraHeight * 1.3f;
			mainCamera->SetEye(_eye);

			//�J���������_������
			XMFLOAT3 _target;
			_target = _player->GetPosition();
			_target.y += cameraHeight;
			mainCamera->SetTarget(_target);
			Wrapper::GetInstance()->SetCamera(mainCamera);

			mainCamera->Update();
		}

		//�X�y�[�X�������ŃQ�[���X�^�[�g
		static int count = 0;
		if (Input::GetInstance()->Push(DIK_SPACE)) {
			count++;
			if (count >= 120) { GameModeNum = GameMode::PLAY; }
		}
	}
	//�Q�[���i�s������
	else if (GameModeNum == GameMode::PLAY) {
		if (StageManager::GetInstance()->GetStage() != GameLocation::BaseCamp) {
			enemy->Update(_player->GetPos(), defense_facilities, StageManager::GetInstance()->GetBaseCamp(0)->position);
		}

		_player->Update();

		//�J�������[�N(�Ǐ])
		{
			//�J�����ƃv���C���[�̋���������
			const float distanceFromPlayerToCamera = 50.0f;

			//�J�����̍���
			const float cameraHeight = 30.0f;

			XMMATRIX rotM = {};
			XMVECTOR vv0 = {};

			//�J�����̐��ʃx�N�g��������
			vv0 = { 0.0f,0.0f,1.0f,0.0f };
			//�c���̉�]
			rotM = XMMatrixRotationX(XMConvertToRadians(_player->GetAngleVertical()));
			vv0 = XMVector3TransformNormal(vv0, rotM);

			//�����̉�]
			rotM = XMMatrixRotationY(XMConvertToRadians(_player->GetAngleHorizonal()));
			vv0 = XMVector3TransformNormal(vv0, rotM);

			//
			XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

			//�J�����ʒu������
			XMFLOAT3 _eye;
			_eye = _player->GetPosition();
			_eye += direction * distanceFromPlayerToCamera;
			_eye.y += cameraHeight * 1.3f;
			mainCamera->SetEye(_eye);

			//�J���������_������
			XMFLOAT3 _target;
			_target = _player->GetPosition();
			_target.y += cameraHeight;
			mainCamera->SetTarget(_target);
			Wrapper::GetInstance()->SetCamera(mainCamera);

			mainCamera->Update();
		}

		//�����蔻��i�v���C���[ / �G / �ŏI�֖�j
		{
			//�G����̍U��
			for (auto& enemy : enemy->GetWolf())
			{
				//�G���U����ԂłȂ���΃X���[
				if (!enemy.get()->attack) { continue; }

				if (enemy.get()->attackHit) { continue; }

				bool hit = Collision::CheckSqhere2Sqhere(enemy.get()->collision, _player->GetCollision());

				if (hit) {
					_player->SetDamage(true);
					_player->SetDamageVec(enemy.get()->GetPosition());
					enemy.get()->attackHit = true;
				}
			}

			//�����̍U��
			for (auto& enemy : enemy->GetWolf())
			{
				//�������U�����Ă��Ȃ���΃X���[
				if (!_player->GetAttack()) { continue; }

				//���ɍU�����������Ă�����X���[
				if (_player->GetHit()) { continue; }

				bool hit = collisionOBBtoOBB(enemy.get()->GetObb(), _player->GetWeapon()->GetObb());

				if (hit) {
					enemy.get()->SetDamage(true);
					_player->SetHit(true);
					repelCount++;
					UI->SetRepel(repelCount);
				}
			}

			//�Q�[�g�ւ̍U��
			for (auto& enemy : enemy->GetGolem())
			{
				//�G���U����ԂłȂ���΃X���[
				if (!enemy.get()->attack) { continue; }

				XMVECTOR inter;

				bool hit = Collision::CheckSqhere2Triangle(enemy.get()->collision, triangle[0], &inter);

				if (hit) { enemy.get()->SetDamage(true); }
			}
		}

		//�X�V����(�Q�[��)
		{
			//�X�e�[�W�ړ�
			if (StageManager::GetInstance()->GetStage() == GameLocation::BaseCamp && _player->GetPosition().z <= -20.0f) {
				UI->GetFade()->SetFadeIn(true);
				light->SetPointLightActive(0, false);

				//�t�F�[�h�C��
				if (UI->GetFade()->GetFadeIn()) {
					UI->GetFade()->FadeIn();
					if (!UI->GetFade()->GetFadeIn()) {
						UI->GetFade()->SetFadeIn(false);
						UI->GetFade()->SetFadeOut(true);
					}
				}

				//�t�F�[�h�A�E�g
				if (UI->GetFade()->GetFadeOut()) {
					UI->GetFade()->FadeOut();

					StageManager::GetInstance()->SetUseStage(GameLocation::BaseStage);
					_player->SetPosition(XMFLOAT3(200.0f, 0.0f, 100.0f));
					_player->SetAngleH(90.0f);
				}
			}

			//�ꎞ��~
			if (directInput->IsButtonPush(directInput->Button09) || Input::GetInstance()->Trigger(DIK_ESCAPE)) {
				GameModeNum = GameMode::POSE;
				pose = true;
			}

			//�Q�[���N���A����
			if (repelCount >= 15) {
				GameModeNum = GameMode::CLEAR;
				UI->GetFade()->SethalfFade(true);
			}
			//�Q�[���I�[�o�[����
			if (_player->GetStatus()->HP <= 0) {
				GameModeNum = GameMode::OVER;
				UI->GetFade()->SethalfFade(true);
			}
		}

		//�p�[�e�B�N������
		static float createTime = 0.2f;
		if (createTime <= 0.0f) {
			particlemanager->CreateParticle(30, _player->GetPos(), 0.01f, 0.005f, 10, 5.0f, XMFLOAT4(0.5f, 0.36f, 0.2f, 1.0f));

			createTime = 0.2f;
		}
		createTime -= fps;

	}
	//�Q�[���N���A������
	else if (GameModeNum == GameMode::CLEAR)
	{
		UI->GetFade()->halfFade();
		if (UI->GetFade()->GetAlpha() >= 0.7f) {
			result = true;
		}
		if (result) {
			UI->GetClear()->SetClear(true);
		}

		UI->GetFade()->Update();
		UI->GetClear()->Update();

		//�G���h���^�C�g���J��
		if (Input::GetInstance()->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			UI->GetClear()->SetClear(false);
		}
	}
	//�Q�[���I�[�o�[������
	else if (GameModeNum == GameMode::OVER) {
		UI->GetFade()->halfFade();
		if (UI->GetFade()->GetAlpha() >= 0.7f) {
			result = true;
		}
		if (result) {
			UI->GetFade()->SetFailed(true);
		}
		UI->GetFade()->Update();
		UI->GetFailed()->Update();

		//�G���h���^�C�g���J��
		if (Input::GetInstance()->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			UI->GetFailed()->SetFailed(false);
		}
	}
	//�|�[�Y��ʎ�����
	if (GameModeNum == GameMode::POSE) {
		if (!pose) {
			if (directInput->IsButtonUp(directInput->Button09) || Input::GetInstance()->Trigger(DIK_ESCAPE)) {
				GameModeNum = GameMode::PLAY;
			}
		}
		pose = false;
	}
	else if (GameModeNum != GameMode::POSE) {
		Object3Ds::SetCamera(dx12->GetCamera());
		FbxObject3d::SetCamera(dx12->GetCamera());

		UI->GetHp()->SetSize(XMFLOAT2(_player->GetStatus()->HP * 4.5f, 30));

		StageManager::GetInstance()->GetSkydome()->SetPosition(XMFLOAT3(_player->GetPosition().x, _player->GetPosition().y + 450.0f, _player->GetPosition().z));

		particlemanager->Update();
		static int i = 0;
		for (auto& enemy : enemy->GetWolf()) {
			//�g��
			light->SetCircleShadowCasterPos(i, enemy.get()->GetPosition());
			light->SetCircleShadowDir(i, XMVECTOR({ circleShadowDir[0],circleShadowDir[1],circleShadowDir[2],0 }));
			light->SetCircleShadowAtten(i, XMFLOAT3(circleShadowAtten));
			light->SetCircleShadowFacterAngle(i, XMFLOAT2(circleShadowFacterAnlge[1], circleShadowFacterAnlge[2] * enemy.get()->shadowOffset));
			i++;
		}
		i = 0;
		light->Update();
	}
}

void PlayScene::Draw()
{
	// �R�}���h���X�g�̎擾
	Sprite::PreDraw(dx12->CommandList().Get());
	if (load) {
		UIManager::GetInstance()->LoadDraw();
	}

	Sprite::PostDraw();

	if (!loadComplate) { return; }

	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	if (!load) { Singleton_Heap::GetInstance()->FbxTexture = 200; }

	BaseObject::PreDraw(cmdList);

	StageManager::GetInstance()->Draw();

	_player->Draw(cmdList);

	enemy->Draw(cmdList);

	BaseObject::PostDraw();

	// 3D�I�u�W�F�N�g�`��O����
	ParticleManager::PreDraw(cmdList);
	// 3D�I�u�N�W�F�N�g�̕`��
	particlemanager->Draw();

	for (auto& enemy : enemy->GetWolf()) {
		enemy.get()->particle->Draw();
	}
	// 3D�I�u�W�F�N�g�`��㏈��
	ParticleManager::PostDraw();

	// �R�}���h���X�g�̎擾
	Sprite::PreDraw(dx12->CommandList().Get());

	UIManager::GetInstance()->GameDarw();
	UIManager::GetInstance()->FadeDraw();

	if (GameModeNum == Start) {
		UIManager::GetInstance()->StartDraw();
	}

	Sprite::PostDraw();
}

void PlayScene::ShadowDraw()
{
	if (!loadComplate) { return; }

	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	if (!load) { Singleton_Heap::GetInstance()->FbxTexture = 200; }

	//�[�x�o�b�t�@�N���A
	dx12->ClearDepthShadow();

	BaseObject::PreDraw(cmdList);

	StageManager::GetInstance()->ShadowDraw();

	_player->ShadowDraw(cmdList);

	BaseObject::PostDraw();
}

void PlayScene::asyncLoad()
{
	auto th = std::thread([&] {LoadResource(); });

	th.join();

	_loadMode = LoadMode::End;
}

void PlayScene::LoadResource()
{
	//�v���C���[---------------------
	_player = Player::Create(FbxLoader::GetInstance()->LoadModelFromFile("testModel"));
	_player->CreateWeapon(Model::CreateFromOBJ("weapon"));
	_player->SetScale({ 0.2f, 0.2f, 0.2f });
	_player->SetPosition(XMFLOAT3(0.0f, 0.0f, 100.0f));
	_player->PlayAnimation();

	//�G�l�~�[-----------------------
	enemy = new EnemyManager();
	enemy = EnemyManager::Create(FbxLoader::GetInstance()->LoadModelFromFile("Golem"), FbxLoader::GetInstance()->LoadModelFromFile("Wolf"));

	//�J�����̈ړ����̍쐬
	const float distanceFromPlayerToCamera = 10.0f;//�J�����ƃv���C���[�̋���������
	const float cameraHeight = 25.0f;//�J�����̍���
	vv0 = { -1.0f,0.0f,0.0f,0.0f };//�J�����̐��ʃx�N�g��������
	XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

	afterEye = _player->GetPosition();
	afterEye += direction * distanceFromPlayerToCamera;
	afterEye.y += cameraHeight;
	mainCamera->SetEye(afterEye);
	//�J���������_������
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
			//�����Ȃ��E�E�E
		case LoadMode::Start:
			//���[�f�B���O�n��
			th = std::thread([&] {asyncLoad(); });
			_loadMode = LoadMode::Run;
			break;
		case LoadMode::Run:
			//���[�f�B���O���ɂ�肽������

			UIManager::GetInstance()->LoadUpdate();

			break;
		case LoadMode::End:
			//���[�f�B���O�I���
			th.join();
			load = false;
			loadComplate = true;

		default:
			break;
		}
	}
}
