#include "SceneManager.h"

void SceneManager::Update()
{
	if (nextScene) {

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
