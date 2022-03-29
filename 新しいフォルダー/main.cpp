
//#include "gameManager.h"
#include "application.h"
#include "dx12Wrapper.h"
//#include "object3D.h"
#include "audio.h"
#include "input.h"
#include "delete.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//GameManager gamemanager;

	//auto result = dx12->result();

	//�E�B���h�E�Y������
	Application app;
	if (!app.Init()) {
		assert(0);
		return 1;
	}
	//DirectX������
	Wrapper dx12;
	if (!dx12.Init(app._hwnd(), app.GetWindowSize())) {
		assert(0);
		return 1;
	}
	//����������
	Audio audio;
	if (!audio.Initalize()) {
		assert(0);
		return 1;
	}
	//�L�[�{�[�h������
	Input input;
	if (!input.Initalize(app._windowClass(), app._hwnd())) {
		assert(0);
		return 1;
	}

	//�Q�[�����[�v
	while (true) {
		//app.Run();
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

		dx12.PreRun();

		dx12.PostRun();

		if (input.Push(DIK_ESCAPE)) {
			break;
		}

	}

#pragma region �㏈��
	//�����N���X�͎g��Ȃ��̂ŏ���
	app.Processing();

#pragma endregion

	return 0;
}