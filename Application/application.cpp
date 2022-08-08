#include "application.h"
#include "dx12Wrapper.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

Application::Application()
{
}

//ウィンドウプロシージャ(描かないといけない)
LRESULT Application::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//ウィンドウが破棄されたら呼ばれる
	if (msg == WM_DESTROY) {
		//OSに対して「このアプリはもう終わる」と伝える
		PostQuitMessage(0);
		return 0;
	}

	//ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//ウィンドウ生成
void Application::CreateWindows(HWND& hwnd, WNDCLASSEX& windowClass) {
	//ウィンドウクラスの生成、登録
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = L"DX12Sample";
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&windowClass);

	RECT wrc = { 0,0,window_width,window_height };
	//関数を使ってウィンドウのサイズを補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(windowClass.lpszClassName,//クラス名指定
		L"DirectXgame",	//タイトルバーの文字
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

bool Application::Init() {
	CreateWindows(hwnd, windowClass);
	return true;
}

void Application::Run() {
	//メッセージループ
	MSG msg = {};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//アプリケーションが終わるときにmessageがWM_QUITになる
	//if (msg.message == WM_QUIT) {
	//	break;
	//}
}

void Application::Processing() {
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

Application& Application::Instance() {
	static Application instance;
	return instance;
}

SIZE Application::GetWindowSize() const {
	SIZE winSize;
	winSize.cx = window_width;
	winSize.cy = window_height;
	return winSize;
}

HWND Application::_hwnd() {
	return hwnd;
}

WNDCLASSEX Application::_windowClass() {
	return windowClass;
}
