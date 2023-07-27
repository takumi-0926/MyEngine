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
	//�V�[���ύX�v��
	if (nextScene) {
		//���V�[���I��
		if (Scene) {
			Scene->Finalize();
			delete Scene;
		}

		//�V�[���؂�ւ�
		Scene = nextScene;
		nextScene = nullptr;

		//�V�[���Ɏ�����ݒ�
		Scene->SetSceneManager(this);

		//�V�V�[���̏�����
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
