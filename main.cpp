
#include "gameManager.h"
#include "application.h"
#include "dx12Wrapper.h"
#include "Audio/audio.h"
#include "Input/input.h"
#include "delete.h"
#include "object/baseObject.h"

#include "object/object3D.h"
#include "object/object2d.h"
#include "BillboardObject.h"

#include "object/Model.h"

#include "Sprite/sprite.h"
#include "2d/PostEffect.h"

#include "PMD/PMDmodel.h"
#include "PMD/pmdObject3D.h"
#include "PMD/PMXLoader.h"

#include "FBX/FbxLoader.h"
#include "FBX/FbxModel.h"
#include "FBX/FbxObject3d.h"

#include "Camera\DebugCamera.h"

#include "light\Light.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Application *app = nullptr;
	Wrapper *dx12 = nullptr;
	Audio audio;
	//Input input;
	PostEffect *postEffect = nullptr;

	//��{������
	{
		//�E�B���h�E�Y������
		app = new Application();
		app->CreateWindows();
		//DirectX������
		dx12 = new Wrapper();
		dx12->Init(app->_hwnd(), app->GetWindowSize());

		Input::GetInstance()->Initalize(app);

		//����������
		if (!audio.Initalize()) {
			assert(0);
			return 1;
		}
		//�L�[�{�[�h������
		//if (!input.Initalize(app)) {
		//	assert(0);
		//	return 1;
		//}
		//imgui
		if (ImGui::CreateContext() == nullptr) {
			assert(0);
			return false;
		}
		bool blnResult = ImGui_ImplWin32_Init(app->_hwnd());
		blnResult = ImGui_ImplDX12_Init(
			dx12->GetDevice(),
			3,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			dx12->GetHeapImgui().Get(),
			dx12->GetHeapImgui()->GetCPUDescriptorHandleForHeapStart(),
			dx12->GetHeapImgui()->GetGPUDescriptorHandleForHeapStart());
	}

	Object3Ds obj3d;
	{
		if (!obj3d.StaticInitialize(dx12->GetDevice())) {
			assert(0);
			return 1;
		}
		//object2d::StaticInitialize(dx12->GetDevice());

		BillboardObject::StaticInitialize(dx12->GetDevice());
	}

	if (!ParticleManager::StaticInitialize(dx12->GetDevice(), app->window_width, app->window_height)) {
		assert(0);
		return 1;
	}

	{
		//�X�v���C�g������
		if (!Sprite::staticInitalize(dx12->GetDevice(), app->GetWindowSize())) {
			assert(0);
			return 1;
		}

		//Sprite::loadTexture(100, L"Resources/white1x1.png");
	}

	//Fbx������
	FbxLoader::GetInstance()->Initialize(dx12->GetDevice());

	PMDobject::StaticInitialize(dx12);

	Light::StaticInitalize(dx12->GetDevice());

	GameManager gameScene;
	//�Q�[���V�[��������
	if (!gameScene.Initalize(dx12, &audio, Input::GetInstance())) {
		assert(0);
		return 1;
	}

	postEffect = new PostEffect();
	postEffect->Initialize();


	//�Q�[�����[�v
	while (true) {
		//���b�Z�[�W���[�v
		MSG msg = {};

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//�A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT) {
			break;
		}

		//ImGui
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//input.Update();
		Input::GetInstance()->Update();
		Object3Ds::SetDx12(dx12);
		gameScene.Update();

		//�e
		dx12->PreRunShadow();
		gameScene.ShadowDraw();
		//dx12->PostRun();

		//�����_�[�e�N�X�`���ɕ`��
		postEffect->PreDrawScene(dx12->CommandList().Get());
		gameScene.MainDraw();
		postEffect->PostDrawScene(dx12->CommandList().Get());

		dx12->PreRun();
		
		//�|�X�g�G�t�F�N�g��`��
		postEffect->Draw(dx12->CommandList().Get());

		//�X�v���C�g�`��
		//gameScene.SubDraw();

		ImGui::Render();
		dx12->CommandList()->SetDescriptorHeaps(
			1,
			dx12->GetHeapImgui().GetAddressOf());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dx12->CommandList().Get());
		
		dx12->PostRun();

		//if (input.Push(DIK_ESCAPE)) {
		//	break;
		//}
		//if (Input::GetInstance()->Trigger(DIK_SPACE)) {
		//	break;
		//}
	}

#pragma region �㏈��
	//�����N���X�͎g��Ȃ��̂ŏ���
	app->Processing();

	FbxLoader::GetInstance()->Finalize();

	delete postEffect;

#pragma endregion

	return 0;
}