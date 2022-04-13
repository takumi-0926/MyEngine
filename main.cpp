
#include "gameManager.h"
#include "application.h"
#include "dx12Wrapper.h"
#include "object3D.h"
#include "audio.h"
#include "input.h"
#include "delete.h"
#include "Model.h"
#include "sprite.h"
#include "PMDmodel.h"
#include "pmdObject3D.h"
#include "baseObject.h"
#include "FbxLoader.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Application app;
	Wrapper dx12;
	Audio audio;
	Input input;


	//基本初期化
	{
		//ウィンドウズ初期化
		if (!app.Init()) {
			assert(0);
			return 1;
		}
		//DirectX初期化
		if (!dx12.Init(app._hwnd(), app.GetWindowSize())) {
			assert(0);
			return 1;
		}
		//音声初期化
		if (!audio.Initalize()) {
			assert(0);
			return 1;
		}
		//キーボード初期化
		if (!input.Initalize(&app)) {
			assert(0);
			return 1;
		}
	}

	Model model;
	Object3Ds obj3d;
	{

		//3Dモデルオブジェクト初期化
		if (!model.StaticInitialize(dx12.GetDevice())) {
			assert(0);
			return 1;
		}

		if (!obj3d.StaticInitialize(dx12.GetDevice(), app.GetWindowSize())) {
			assert(0);
			return 1;
		}
	}

	PMDmodel pModel;
	PMDobject pmdObj;
	{
		//PMDモデル初期化
		if (!PMDmodel::StaticInitialize(dx12.GetDevice())) {
			assert(0);
			return 1;
		}
		if (!PMDobject::StaticInitialize(dx12.GetDevice(), app.GetWindowSize())) {
			assert(0);
			return 1;
		}
	}

	{
		//スプライト初期化
		if (!Sprite::staticInitalize(dx12.GetDevice(), app.GetWindowSize())) {
			assert(0);
			return 1;
		}
	}

	//Fbx初期化
	FbxLoader::GetInstance()->Initialize(dx12.GetDevice());

	GameManager gameScene;
	//ゲームシーン初期化
	if (!gameScene.Initalize(&dx12, &audio, &input)) {
		assert(0);
		return 1;
	}


	//ゲームループ
	while (true) {
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

		gameScene.Update();

		dx12.PreRun();

		gameScene.Draw();

		dx12.PostRun();

		if (input.Push(DIK_ESCAPE)) {
			break;
		}
	}

#pragma region 後処理
	//もうクラスは使わないので消去
	app.Processing();

	FbxLoader::GetInstance()->Finalize();

#pragma endregion

	return 0;
}