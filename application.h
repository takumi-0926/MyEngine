#pragma once
#include <Windows.h>
#include "dx12Wrapper.h"

class Application {
private:
	WNDCLASSEX windowClass;
	HWND hwnd;

	//ウィンドウ生成
	void CreateWindows(HWND& hwnd, WNDCLASSEX& windowClass);
public://静的メンバ関数
	static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//ウィンドウ定数
	static const int window_width = 1280;// 横幅
	static const int window_height = 720;// 縦幅
public:

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
