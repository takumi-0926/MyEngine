#include "SceneManager.h"
#include "dx12Wrapper.h"

SceneManager::SceneManager(Wrapper* _dx12):
	dx12(_dx12)
{
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
