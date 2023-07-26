#include "Framework.h"

#include "application.h"
#include "dx12Wrapper.h"
#include "Input/input.h"
#include "Audio/audio.h"
#include "2d/PostEffect.h"

#include "Singleton_Heap.h"
#include "Sprite\sprite.h"
#include "object/object3D.h"
#include "FBX/FbxLoader.h"
#include "FBX/FbxObject3d.h"
#include "ParticleManager.h"

void Framework::Run()
{
	//�Q�[��������
	Initialize();

	//�Q�[�����[�v
	while (true) {

		//�X�V
		Update();

		if (IsEndReqest()) {
			break;
		}

		//�`��
		Draw();
	}

#pragma region �㏈��

	Finalize();

#pragma endregion
}

void Framework::Initialize()
{
	//��{������
	//�E�B���h�E�Y������
	app = new Application();
	app->CreateWindows();
	app->InitFps();

	//DirectX������
	dx12 = new Wrapper();
	dx12->Init(app->_hwnd(), app->GetWindowSize());
	Wrapper::GetInstance()->Init(app->_hwnd(), app->GetWindowSize());
	Input::GetInstance()->Initalize(app);
	//����������
	Audio::GetInstance()->Initalize();

	//imgui
	if (ImGui::CreateContext() == nullptr) { assert(0); }
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

	Object3Ds obj3d;
	if (!obj3d.StaticInitialize(Singleton_Heap::GetInstance()->GetDevice())) { assert(0); }

	//Fbx������
	FbxLoader::GetInstance()->Initialize(Singleton_Heap::GetInstance()->GetDevice());
	FbxObject3d::staticInitialize(Singleton_Heap::GetInstance()->GetDevice());

	if (!ParticleManager::StaticInitialize(Singleton_Heap::GetInstance()->GetDevice(), app->window_width, app->window_height)) {
		assert(0);
	}

	//�X�v���C�g������
	if (!Sprite::staticInitalize(Singleton_Heap::GetInstance()->GetDevice(), app->GetWindowSize())) {
		assert(0);
	}


	PMDobject::StaticInitialize(dx12);

	Light::StaticInitalize(Singleton_Heap::GetInstance()->GetDevice());

}

void Framework::Finalize()
{
}

void Framework::Update()
{
	//���b�Z�[�W���[�v
	MSG msg = {};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//�A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
	if (msg.message == WM_QUIT) {
		endReqest = true;
		return;
	}

	//ImGui
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Input::GetInstance()->Update();
}
