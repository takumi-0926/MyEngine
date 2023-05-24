
#include "gameManager.h"
#include "application.h"
#include "dx12Wrapper.h"
#include "..\Singleton_Heap.h"
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
		app->InitFps();
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
		auto ImguiCPU = Singleton_Heap::GetInstance()->GetDescHeap()->GetCPUDescriptorHandleForHeapStart();
		ImguiCPU.ptr += Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 3;
		auto ImguiGPU = Singleton_Heap::GetInstance()->GetDescHeap()->GetGPUDescriptorHandleForHeapStart();
		ImguiGPU.ptr += Singleton_Heap::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 3;

		blnResult = ImGui_ImplDX12_Init(
			Singleton_Heap::GetInstance()->GetDevice(),
			3,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			Singleton_Heap::GetInstance()->GetDescHeap(),
			ImguiCPU,
			ImguiGPU);
	}

	Object3Ds obj3d;
	{
		if (!obj3d.StaticInitialize(Singleton_Heap::GetInstance()->GetDevice())) {
			assert(0);
			return 1;
		}

		BillboardObject::StaticInitialize(Singleton_Heap::GetInstance()->GetDevice());
	}

	//Fbx������
	FbxLoader::GetInstance()->Initialize(Singleton_Heap::GetInstance()->GetDevice());
	FbxObject3d::staticInitialize(Singleton_Heap::GetInstance()->GetDevice());

	if (!ParticleManager::StaticInitialize(Singleton_Heap::GetInstance()->GetDevice(), app->window_width, app->window_height)) {
		assert(0);
		return 1;
	}

	{
		//�X�v���C�g������
		if (!Sprite::staticInitalize(Singleton_Heap::GetInstance()->GetDevice(), app->GetWindowSize())) {
			assert(0);
			return 1;
		}

		//Sprite::loadTexture(100, L"Resources/white1x1.png");
	}

	PMDobject::StaticInitialize(dx12);

	Light::StaticInitalize(Singleton_Heap::GetInstance()->GetDevice());

	GameManager gameScene;
	//�Q�[���V�[��������
	if (!gameScene.Initalize(dx12, &audio, Input::GetInstance())) {
		assert(0);
		return 1;
	}

	postEffect = new PostEffect();
	postEffect->Initialize(Singleton_Heap::GetInstance()->GetDescHeap());


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

		app->CalculationFps();

		//ImGui
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//input.Update();
		Input::GetInstance()->Update();
		Object3Ds::SetDx12(dx12);
		gameScene.Update();

		Singleton_Heap::GetInstance()->FbxTexture = 200;

		//�e
		dx12->PreRunShadow();
		gameScene.ShadowDraw();
		//dx12->PostRun();

		Singleton_Heap::GetInstance()->FbxTexture = 200;

		//�����_�[�e�N�X�`���ɕ`��
		postEffect->PreDrawScene(dx12->CommandList().Get());
		gameScene.MainDraw();
		postEffect->PostDrawScene(dx12->CommandList().Get());

		dx12->PreRun();
		
		//�|�X�g�G�t�F�N�g��`��
		postEffect->Draw(dx12->CommandList().Get(), Singleton_Heap::GetInstance()->GetDescHeap());

		//�X�v���C�g�`��
		gameScene.SubDraw();

		ImGui::Render();
		ID3D12DescriptorHeap* heaps[] = { Singleton_Heap::GetInstance()->GetDescHeap() };
		dx12->CommandList()->SetDescriptorHeaps(
			1,
			heaps);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dx12->CommandList().Get());
		
		dx12->PostRun();

		app->CalculationSleep();
	}

#pragma region �㏈��
	//�����N���X�͎g��Ȃ��̂ŏ���
	app->Processing();

	FbxLoader::GetInstance()->Finalize();

	delete postEffect;

#pragma endregion

	return 0;
}