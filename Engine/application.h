#pragma once
#include <Windows.h>

class Application {
private://メンバ変数
	WNDCLASSEX windowClass{};
	HWND hwnd = nullptr;

public://静的メンバ変数

	//ウィンドウ定数
	static const int window_width = 1280;// 横幅
	static const int window_height = 720;// 縦幅
	static const wchar_t crassName[];    // クラスネーム
public:

	//コンストラクタ
	Application();


	///Applicationのシングルトンインスタンスを得る
	static Application& Instance();

	static LRESULT WindowProcedure(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

	//ウィンドウ生成
	void CreateWindows();

	/// <summary>
	/// メッセージ
	/// </summary>
	/// <returns>終了判定</returns>
	bool processMessage();

	//終了時処理
	void Processing();

	//ゲッター
	SIZE GetWindowSize()const {
		SIZE winSize;
		winSize.cx = window_width;
		winSize.cy = window_height;
		return winSize;
	};
	HWND _hwnd() { return hwnd; }
	HINSTANCE classInstance() { return windowClass.hInstance; };
};
