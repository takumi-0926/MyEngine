
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

	//ウィンドウズ初期化
	Application app;
	if (!app.Init()) {
		assert(0);
		return 1;
	}
	//DirectX初期化
	Wrapper dx12;
	if (!dx12.Init(app._hwnd(), app.GetWindowSize())) {
		assert(0);
		return 1;
	}
	//音声初期化
	Audio audio;
	if (!audio.Initalize()) {
		assert(0);
		return 1;
	}
	//キーボード初期化
	Input input;
	if (!input.Initalize(app._windowClass(), app._hwnd())) {
		assert(0);
		return 1;
	}

	//ゲームループ
	while (true) {
		//app.Run();
		//メッセージループ
		MSG msg = {};

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//アプリケーションが終わるときにmessageがWM_QUITになる
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

#pragma region 後処理
	//もうクラスは使わないので消去
	app.Processing();

#pragma endregion

	return 0;
}