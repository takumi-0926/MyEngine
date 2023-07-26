#include "PlayScene.h"
#include "Player.h"
#include "Camera/DebugCamera.h"

#include "UIManager.h"
#include "EnemyManager.h"
#include "StageManager.h"
#include "Math/MyMath.h"

#include <future>
#include <Singleton_Heap.h>

static const int debugTextTexNumber = 99;
std::thread th = {};

void PlayScene::Initialize(Wrapper* _dx12)
{
	assert(_dx12);

	this->dx12 = _dx12;

	//基本オブジェクト--------------
	defenceModel = Model::CreateFromOBJ("KSR-29");
	bulletModel = Model::CreateFromOBJ("bullet");

	//カメラをセット
	mainCamera = new Camera(Application::window_width, Application::window_height);
	setCamera = new Camera(Application::window_width, Application::window_height);

	Wrapper::SetCamera(titleCamera);
	FbxObject3d::SetCamera(dx12->Camera());
	Object3Ds::SetCamera(dx12->Camera());

	dx12->SceneUpdate();

	//光源
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
	//ライトセット
	Wrapper::SetLight(light);
}

void PlayScene::Update()
{
	mainCamera->Update();
	setCamera->Update();

	//ライト
	light->SetPointLightPos(0,
		XMFLOAT3(StageManager::GetInstance()->GetBaseCamp(29)->position.x,
			StageManager::GetInstance()->GetBaseCamp(29)->position.y + 5.0f,
			StageManager::GetInstance()->GetBaseCamp(29)->position.z));
	
	//フェードイン
	if (UIManager::GetInstance()->GetFade()->GetFadeIn()) {
		UIManager::GetInstance()->GetFade()->FadeIn();
		if (!UIManager::GetInstance()->GetFade()->GetFadeIn()) {
			Wrapper::SetCamera(mainCamera);
			UIManager::GetInstance()->GetFade()->SetFadeIn(false);
			UIManager::GetInstance()->GetFade()->SetFadeOut(true);

			load = true;
		}
	}

	if (load) {
		loading();

		if (!load) { SceneNum = Scene::GAME; }
	}

	keyFlag = false;

}

void PlayScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = Wrapper::GetInstance()->CommandList().Get();

	if (!load) { Singleton_Heap::GetInstance()->FbxTexture = 200; }

	BaseObject::PreDraw(cmdList);

	StageManager::GetInstance()->Draw();

	BaseObject::PostDraw();

	// コマンドリストの取得
	Sprite::PreDraw(Wrapper::GetInstance()->CommandList().Get());

	UIManager::GetInstance()->GameDarw();

	if (load) {
		UIManager::GetInstance()->LoadDraw();
	}

	Sprite::PostDraw();

}

void PlayScene::ShadowDraw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dx12->CommandList().Get();

	if (!load) { Singleton_Heap::GetInstance()->FbxTexture = 200; }

	//深度バッファクリア
	dx12->ClearDepthShadow();

	BaseObject::PreDraw(cmdList);

	StageManager::GetInstance()->ShadowDraw();

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
	//プレイヤー---------------------
	_player = Player::Create(FbxLoader::GetInstance()->LoadModelFromFile("testModel"));
	_player->CreateWeapon(Model::CreateFromOBJ("weapon"));
	_player->SetScale({ 0.2f, 0.2f, 0.2f });
	_player->SetPosition(XMFLOAT3(0.0f, 0.0f, 100.0f));
	_player->PlayAnimation();

	//エネミー-----------------------
	enemy = new EnemyManager();
	enemy = EnemyManager::Create(FbxLoader::GetInstance()->LoadModelFromFile("Golem"), FbxLoader::GetInstance()->LoadModelFromFile("Wolf"));

	//カメラの移動元の作成
	const float distanceFromPlayerToCamera = 10.0f;//カメラとプレイヤーの距離を決定
	const float cameraHeight = 25.0f;//カメラの高さ
	vv0 = { -1.0f,0.0f,0.0f,0.0f };//カメラの正面ベクトルを決定
	XMFLOAT3 direction = { vv0.m128_f32[0],vv0.m128_f32[1],vv0.m128_f32[2] };

	afterEye = _player->GetPosition();
	afterEye += direction * distanceFromPlayerToCamera;
	afterEye.y += cameraHeight;
	mainCamera->SetEye(afterEye);
	//カメラ注視点を決定
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
			//何もない・・・
		case LoadMode::Start:
			//ローディング始め
			th = std::thread([&] {asyncLoad(); });
			_loadMode = LoadMode::Run;
			break;
		case LoadMode::Run:
			//ローディング中にやりたいこと

			UIManager::GetInstance()->LoadUpdate();

			break;
		case LoadMode::End:
			//ローディング終わり
			th.join();
			load = false;

		default:
			break;
		}
	}
}
