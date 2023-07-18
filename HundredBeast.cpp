#include "HundredBeast.h"
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
#include "gameManager.h"

void HundredBeast::Initialize()
{
	//��{������
		//�E�B���h�E�Y������
	app = new Application();
	app->CreateWindows();
	app->InitFps();

	//DirectX������
	dx12 = new Wrapper();
	dx12->Init(app->_hwnd(), app->GetWindowSize());

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

	//�Q�[���V�[��������
	gameScene = new GameManager();
	if (!gameScene->Initalize(dx12, Audio::GetInstance(), Input::GetInstance())) { assert(0); }

	postEffect = new PostEffect();
	postEffect->Initialize(Singleton_Heap::GetInstance()->GetDescHeap());

}

void HundredBeast::Finalize()
{
	//�����N���X�͎g��Ȃ��̂ŏ���
	app->Processing();

	FbxLoader::GetInstance()->Finalize();

	delete postEffect;

}

void HundredBeast::Update()
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

	app->CalculationFps();

	//ImGui
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//input.Update();
	Input::GetInstance()->Update();
	Object3Ds::SetDx12(dx12);
	gameScene->Update();

	app->CalculationSleep();

}

void HundredBeast::Draw()
{
	//�e
	dx12->PreRunShadow();
	gameScene->ShadowDraw();
	//dx12->PostRun();

	//�����_�[�e�N�X�`���ɕ`��
	postEffect->PreDrawScene(dx12->CommandList().Get());
	gameScene->MainDraw();
	postEffect->PostDrawScene(dx12->CommandList().Get());

	dx12->PreRun();

	//�|�X�g�G�t�F�N�g��`��
	postEffect->Draw(dx12->CommandList().Get(), Singleton_Heap::GetInstance()->GetDescHeap());

	//�X�v���C�g�`��
	gameScene->SubDraw();

	ImGui::Render();
	ID3D12DescriptorHeap* heaps[] = { Singleton_Heap::GetInstance()->GetDescHeap() };
	dx12->CommandList()->SetDescriptorHeaps(
		1,
		heaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dx12->CommandList().Get());

	dx12->PostRun();

	app->CalculationSleep();
}
