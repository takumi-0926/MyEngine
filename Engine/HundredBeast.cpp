#include "HundredBeast.h"
#include "application.h"
#include "dx12Wrapper.h"
#include "Input/input.h"
#include "Audio/audio.h"
#include "2d/PostEffect.h"

#include "Singleton_Heap.h"
#include "Sprite/sprite.h"
#include "object/object3D.h"
#include "FBX/FbxLoader.h"
#include "FBX/FbxObject3d.h"
#include "ParticleManager.h"

#include "TitleScene.h"
#include "PlayScene.h"
#include "UIManager.h"
#include "StageManager.h"

void HundredBeast::Initialize()
{
	//���N���X�̏�����
	Framework::Initialize();

	//�V�[��
	BsScene* scene = new TitleScene();
	sceneManager->SetNextScene(scene);

	//UI�X�v���C�g
	UIManager::GetInstance()->Initialize();
	//�X�e�[�W
	StageManager::GetInstance()->Initialize();
}

void HundredBeast::Finalize()
{
	Framework::Finalize();
}

void HundredBeast::Update()
{
	app->CalculationFps();

	Framework::Update();

	app->CalculationSleep();
}

void HundredBeast::Draw()
{
	Framework::Draw();
}
