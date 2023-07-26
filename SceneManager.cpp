#include "SceneManager.h"

void SceneManager::Update()
{
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
