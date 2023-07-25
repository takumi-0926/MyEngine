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
//#include "gameManager.h"
#include "PlayScene.h"

void HundredBeast::Initialize()
{
	//���N���X�̏�����
	Framework::Initialize();

	//�Q�[���V�[��������
	//gameScene = new GameManager();
	//if (!gameScene->Initalize(dx12, Audio::GetInstance(), Input::GetInstance())) { assert(0); }

	Scene = new PlayScene();
	Scene->Initialize(dx12);

	postEffect = new PostEffect();
	postEffect->Initialize(Singleton_Heap::GetInstance()->GetDescHeap());
}

void HundredBeast::Finalize()
{
	Framework::Finalize();

	//�����N���X�͎g��Ȃ��̂ŏ���
	app->Processing();

	delete dx12;

	FbxLoader::GetInstance()->Finalize();

	delete postEffect;

}

void HundredBeast::Update()
{
	app->CalculationFps();

	Framework::Update();


	Object3Ds::SetDx12(dx12);
	//gameScene->Update();
	Scene->Update();

	app->CalculationSleep();
}

void HundredBeast::Draw()
{

	//�e
	dx12->PreRunShadow();
	//gameScene->ShadowDraw();
	Scene->ShadowDraw();

	//�����_�[�e�N�X�`���ɕ`��
	postEffect->PreDrawScene(dx12->CommandList().Get());
	//gameScene->MainDraw();
	Scene->Draw();
	postEffect->PostDrawScene(dx12->CommandList().Get());

	dx12->PreRun();

	//�|�X�g�G�t�F�N�g��`��
	postEffect->Draw(dx12->CommandList().Get(), Singleton_Heap::GetInstance()->GetDescHeap());

	//�X�v���C�g�`��
	//gameScene->SubDraw();

	ImGui::Render();
	ID3D12DescriptorHeap* heaps[] = { Singleton_Heap::GetInstance()->GetDescHeap() };
	dx12->CommandList()->SetDescriptorHeaps(
		1,
		heaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dx12->CommandList().Get());

	dx12->PostRun();

	app->CalculationSleep();
}
