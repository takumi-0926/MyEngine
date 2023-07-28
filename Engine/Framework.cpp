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

#include "SceneManager.h"

void Framework::Run()
{
	//ゲーム初期化
	Initialize();

	//ゲームループ
	while (true) {

		//更新
		Update();

		if (IsEndReqest()) {
			break;
		}

		//描画
		Draw();
	}

#pragma region 後処理

	Finalize();

#pragma endregion
}

void Framework::Initialize()
{
	//基本初期化
	//ウィンドウズ初期化
	app = new Application();
	app->CreateWindows();
	app->InitFps();

	//DirectX初期化
	dx12 = new Wrapper();
	dx12->Init(app->_hwnd(), app->GetWindowSize());
	//入力初期化
	Input::GetInstance()->Initalize(app);
	//音声初期化
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

	//Fbx初期化
	FbxLoader::GetInstance()->Initialize(Singleton_Heap::GetInstance()->GetDevice());
	FbxObject3d::staticInitialize(Singleton_Heap::GetInstance()->GetDevice());

	if (!ParticleManager::StaticInitialize(Singleton_Heap::GetInstance()->GetDevice(), app->window_width, app->window_height)) {
		assert(0);
	}

	//スプライト初期化
	if (!Sprite::staticInitalize(Singleton_Heap::GetInstance()->GetDevice(), app->GetWindowSize())) {
		assert(0);
	}


	PMDobject::StaticInitialize(dx12);

	Light::StaticInitalize(Singleton_Heap::GetInstance()->GetDevice());

	//ポストエフェクト
	postEffect = new PostEffect();
	postEffect->Initialize(Singleton_Heap::GetInstance()->GetDescHeap());

	sceneManager = new SceneManager(dx12);
}

void Framework::Finalize()
{
	//もうクラスは使わないので消去
	app->Processing();

	delete sceneManager;

	delete dx12;

	FbxLoader::GetInstance()->Finalize();

	delete postEffect;
}

void Framework::Update()
{
	//メッセージループ
	MSG msg = {};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//アプリケーションが終わるときにmessageがWM_QUITになる
	if (msg.message == WM_QUIT) {
		endReqest = true;
		return;
	}

	//ImGui
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Input::GetInstance()->Update();

	sceneManager->Update();
}

void Framework::Draw()
{
	Object3Ds::SetDx12(dx12);

	//影
	dx12->PreRunShadow();
	sceneManager->ShadowDraw();

	//レンダーテクスチャに描画
	postEffect->PreDrawScene(dx12->CommandList().Get());
	sceneManager->Draw();
	postEffect->PostDrawScene(dx12->CommandList().Get());

	dx12->PreRun();

	//ポストエフェクトを描画
	postEffect->Draw(dx12->CommandList().Get(), Singleton_Heap::GetInstance()->GetDescHeap());

	//スプライト描画

	ImGui::Render();
	ID3D12DescriptorHeap* heaps[] = { Singleton_Heap::GetInstance()->GetDescHeap() };
	dx12->CommandList()->SetDescriptorHeaps(
		1,
		heaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dx12->CommandList().Get());

	dx12->PostRun();

	app->CalculationSleep();
}
