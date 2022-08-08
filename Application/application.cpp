#include "application.h"
#include "dx12Wrapper.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

Application::Application()
{
}

//�E�B���h�E�v���V�[�W��(�`���Ȃ��Ƃ����Ȃ�)
LRESULT Application::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//�E�B���h�E���j�����ꂽ��Ă΂��
	if (msg == WM_DESTROY) {
		//OS�ɑ΂��āu���̃A�v���͂����I���v�Ɠ`����
		PostQuitMessage(0);
		return 0;
	}

	//ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//�E�B���h�E����
void Application::CreateWindows(HWND& hwnd, WNDCLASSEX& windowClass) {
	//�E�B���h�E�N���X�̐����A�o�^
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = L"DX12Sample";
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&windowClass);

	RECT wrc = { 0,0,window_width,window_height };
	//�֐����g���ăE�B���h�E�̃T�C�Y��␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(windowClass.lpszClassName,//�N���X���w��
		L"DirectXgame",	//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,		//�\��x���W��OS�ɂ��C��
		CW_USEDEFAULT,		//�\��y���W��OS�ɂ��C��
		wrc.right - wrc.left,//�E�B���h�E��
		wrc.bottom - wrc.top,//�E�B���h�E��
		nullptr,			//�e�E�B���h�E�n���h��
		nullptr,			//���j���[�n���h��
		windowClass.hInstance,		//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);			//�ǉ��p�����[�^�[

	//�E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
}

bool Application::Init() {
	CreateWindows(hwnd, windowClass);
	return true;
}

void Application::Run() {
	//���b�Z�[�W���[�v
	MSG msg = {};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//�A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
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
