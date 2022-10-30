#include "application.h"
#include "dx12Wrapper.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

const wchar_t Application::crassName[] = L"MyGameEngine";

Application::Application()
{
}

//ウィンドウプロシージャ(描かないといけない)
LRESULT Application::WindowProcedure(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
	//ウィンドウが破棄されたら呼ばれる
	if (_msg == WM_DESTROY) {
		//OSに対して「このアプリはもう終わる」と伝える
		PostQuitMessage(0);
		return 0;
	}

	if (ImGui_ImplWin32_WndProcHandler(_hwnd, _msg, _wparam, _lparam)) {
		return 1;
	}

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}

//ウィンドウ生成
void Application::CreateWindows() {
	//ウィンドウクラスの生成、登録
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = crassName;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&windowClass);

	RECT wrc = { 0,0,window_width,window_height };
	//関数を使ってウィンドウのサイズを補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(windowClass.lpszClassName,//クラス名指定
		crassName,	//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,		//表示x座標はOSにお任せ
		CW_USEDEFAULT,		//表示y座標はOSにお任せ
		wrc.right - wrc.left,//ウィンドウ幅
		wrc.bottom - wrc.top,//ウィンドウ高
		nullptr,			//親ウィンドウハンドル
		nullptr,			//メニューハンドル
		windowClass.hInstance,		//呼び出しアプリケーションハンドル
		nullptr);			//追加パラメーター

	//ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);
}



void Application::Processing() {
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

Application& Application::Instance() {
	static Application instance;
	return instance;
}

bool Application::processMessage()
{
	//メッセージループ
	MSG msg = {};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//アプリケーションが終わるときにmessageがWM_QUITになる
	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}
