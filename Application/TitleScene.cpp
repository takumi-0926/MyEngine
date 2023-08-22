#include "TitleScene.h"
#include "PlayScene.h"
#include "SceneManager.h"

#include <future>
#include <Singleton_Heap.h>

#include "Camera/Camera.h"
#include "Input/input.h"
#include "dx12Wrapper.h"
#include "FBX/FbxObject3d.h"
#include "object/object3D.h"

void TitleScene::Initialize(Wrapper* _dx12)
{
	assert(&_dx12);

	this->dx12 = _dx12;

	titleCamera = new Camera(Application::window_width, Application::window_height);

	titleCamera->SetEye(XMFLOAT3(0, 30.0f, -20.0f));
	titleCamera->SetTarget(XMFLOAT3(0, 30.0f, 0.0f));
	titleCamera->Update();

	Wrapper::SetCamera(titleCamera);
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

	StageManager::GetInstance()->SetUseStage(GameLocation::TitleStage);
}

void TitleScene::Finalize()
{
	delete light;
	delete titleCamera;
}

void TitleScene::Update()
{
	titleCamera->Update();

	Object3Ds::SetCamera(titleCamera);

	StageManager::GetInstance()->Update();

	//�^�C�g���i�x0
	if (UIManager::GetInstance()->GetWave() == 0) {
		if (Input::GetInstance()->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) {
			UIManager::GetInstance()->SetWave(1);
			UIManager::GetInstance()->SetStart(true);
			UIManager::GetInstance()->SetOption(false);

			keyFlag = true;
		}
	}
	//�^�C�g���i�x1
	else if (UIManager::GetInstance()->GetWave() == 1) {

		//���ڑI���i�X�^�[�g�A�I�v�V�����j
		if (directInput->IsButtonPush(DirectInput::ButtonKind::DownButton) && UIManager::GetInstance()->GetHieraruchy() < 1) { UIManager::GetInstance()->SetHieraruchy(1); }
		if (directInput->IsButtonPush(DirectInput::ButtonKind::UpButton) && UIManager::GetInstance()->GetHieraruchy() >= 1) { UIManager::GetInstance()->SetHieraruchy(0); }

		//�X�^�[�g�I�����
		if (UIManager::GetInstance()->GetHieraruchy() == 0) {
			UIManager::GetInstance()->SetStart(true);
			UIManager::GetInstance()->SetOption(false);

			//�^�C�g������Q�[����
			if ((Input::GetInstance()->Trigger(DIK_SPACE) || directInput->IsButtonPush(DirectInput::ButtonKind::ButtonA)) && !keyFlag) {
				UIManager::GetInstance()->GetFade()->SetFadeIn(true);
				UIManager::GetInstance()->GetStarted()->SetFadeIn(true);
			}
		}
		//�I�v�V�����I�����
		else if (UIManager::GetInstance()->GetHieraruchy() == 1) {
			UIManager::GetInstance()->SetStart(false);
			UIManager::GetInstance()->SetOption(true);
		}
	}

	//�t�F�[�h�C��
	if (UIManager::GetInstance()->GetFade()->GetFadeIn()) {
		UIManager::GetInstance()->GetFade()->FadeIn();
		if (!UIManager::GetInstance()->GetFade()->GetFadeIn()) {
			UIManager::GetInstance()->GetFade()->SetFadeIn(false);
			UIManager::GetInstance()->GetFade()->SetFadeOut(true);

			//�V�[���؂�ւ�
			BsScene* scene = new PlayScene();
			sceneManager->SetNextScene(scene);
		}
	}

	keyFlag = false;

	////�t�F�[�h�C��
	//if (UIManager::GetInstance()->GetFade()->GetFadeIn()) {
	//	UIManager::GetInstance()->GetFade()->FadeIn();
	//	if (!UIManager::GetInstance()->GetFade()->GetFadeIn()) {
	//		Wrapper::SetCamera(mainCamera);
	//		UIManager::GetInstance()->GetFade()->SetFadeIn(false);
	//		UIManager::GetInstance()->GetFade()->SetFadeOut(true);

	//		load = true;
	//	}
	//}
}

void TitleScene::Draw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	BaseObject::PreDraw(cmdList);

	StageManager::GetInstance()->Draw();

	BaseObject::PostDraw();

	// �R�}���h���X�g�̎擾
	Sprite::PreDraw(dx12->CommandList().Get());

	UIManager::GetInstance()->TitleDarw();
	UIManager::GetInstance()->FadeDraw();

	Sprite::PostDraw();
}

void TitleScene::ShadowDraw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	//�[�x�o�b�t�@�N���A
	dx12->ClearDepthShadow();

	BaseObject::PreDraw(cmdList);

	StageManager::GetInstance()->ShadowDraw();

	BaseObject::PostDraw();
}