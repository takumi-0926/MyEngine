#include "SceneManager.h"
#include "dx12Wrapper.h"

SceneManager::SceneManager(Wrapper* _dx12):
	dx12(_dx12)
{
}

SceneManager::~SceneManager()
{
	Scene->Finalize();
	delete Scene;
}

void SceneManager::Update()
{
	//シーン変更要求
	if (nextScene) {
		//旧シーン終了
		if (Scene) {
			Scene->Finalize();
			delete Scene;
		}

		//シーン切り替え
		Scene = nextScene;
		nextScene = nullptr;

		//シーンに自分を設定
		Scene->SetSceneManager(this);

		//新シーンの初期化
		Scene->Initialize(dx12);
	}

	Scene->Update();
}

void SceneManager::Draw()
{
	Scene->Draw();
}

void SceneManager::ShadowDraw()
{
	Scene->ShadowDraw();
}
