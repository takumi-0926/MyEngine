#pragma once
#include "includes.h"
#include "dx12Wrapper.h"

class Application {
private:
	WNDCLASSEX windowClass;
	HWND hwnd;
		
	//ウィンドウ生成
	void CreateWindows(HWND& hwnd, WNDCLASSEX& windowClass);
public:
	//ウィンドウ定数
	static const int window_width = 1280;// 横幅
	static const int window_height = 720;// 縦幅

	Application();

	//初期化
	bool Init();

	//ループ
	void Run();

	//後処理
	void Processing();

	///Applicationのシングルトンインスタンスを得る
	static Application& Instance();

	//ウィンドウサイズの取得
	SIZE GetWindowSize()const;

	HWND _hwnd();
	WNDCLASSEX _windowClass();
};
