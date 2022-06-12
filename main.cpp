
#include "gameManager.h"
#include "application.h"
#include "dx12Wrapper.h"
#include "audio.h"
#include "input.h"
#include "delete.h"
#include "baseObject.h"

#include "object3D.h"
#include "Model.h"

#include "sprite.h"
#include "2d/PostEffect.h"

#include "PMDmodel.h"
#include "pmdObject3D.h"

#include "FbxLoader.h"
#include "FbxModel.h"
#include "FbxObject3d.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Application app;
	Wrapper dx12;
	Audio audio;
	Input input;
	PostEffect *postEffect = nullptr;

	//��{������
	{
		//�E�B���h�E�Y������
		if (!app.Init()) {
			assert(0);
			return 1;
		}
		//DirectX������
		if (!dx12.Init(app._hwnd(), app.GetWindowSize())) {
			assert(0);
			return 1;
		}
		//����������
		if (!audio.Initalize()) {
			assert(0);
			return 1;
		}
		//�L�[�{�[�h������
		if (!input.Initalize(&app)) {
			assert(0);
			return 1;
		}
	}

	Model model;
	Object3Ds obj3d;
	{

		//3D���f���I�u�W�F�N�g������
		if (!model.StaticInitialize(dx12.GetDevice())) {
			assert(0);
			return 1;
		}

		if (!obj3d.StaticInitialize(dx12.GetDevice(), app.GetWindowSize())) {
			assert(0);
			return 1;
		}
	}

	//PMDmodel pModel;
	//PMDobject pmdObj;
	//{
	//	//PMD���f��������
	//	if (!PMDmodel::StaticInitialize(dx12.GetDevice())) {
	//		assert(0);
	//		return 1;
	//	}
	//	if (!PMDobject::StaticInitialize(dx12.GetDevice(), app.GetWindowSize())) {
	//		assert(0);
	//		return 1;
	//	}
	//}

	//FbxModel fbxModel;
	//FbxObject fbxObj;

	{
		//�X�v���C�g������
		if (!Sprite::staticInitalize(dx12.GetDevice(), app.GetWindowSize())) {
			assert(0);
			return 1;
		}

		Sprite::loadTexture(100, L"Resoureces/white1x1.png");
		postEffect = new PostEffect();
		postEffect->Initalize();
	}

	//Fbx������
	FbxLoader::GetInstance()->Initialize(dx12.GetDevice());

	GameManager gameScene;
	//�Q�[���V�[��������
	if (!gameScene.Initalize(&dx12, &audio, &input)) {
		assert(0);
		return 1;
	}


	//�Q�[�����[�v
	while (true) {
		//���b�Z�[�W���[�v
		MSG msg = {};

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//�A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT) {
			break;
		}

		input.Update();

		gameScene.Update();

		dx12.PreRun();

		postEffect->Draw(dx12.CommandList().Get());

		gameScene.Draw();

		dx12.PostRun();

		if (input.Push(DIK_ESCAPE)) {
			break;
		}
	}

#pragma region �㏈��
	//�����N���X�͎g��Ȃ��̂ŏ���
	app.Processing();

	FbxLoader::GetInstance()->Finalize();

	delete postEffect;

#pragma endregion

	return 0;
}