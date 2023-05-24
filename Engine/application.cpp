#include "application.h"
#include "dx12Wrapper.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

const wchar_t Application::crassName[] = L"�S�b��r(�r�[�X�e�b�h�E�i�C�g�E�N���E�Y)";

Application::Application()
{
}

//�E�B���h�E�v���V�[�W��(�`���Ȃ��Ƃ����Ȃ�)
LRESULT Application::WindowProcedure(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
	//�E�B���h�E���j�����ꂽ��Ă΂��
	if (_msg == WM_DESTROY) {
		//OS�ɑ΂��āu���̃A�v���͂����I���v�Ɠ`����
		PostQuitMessage(0);
		return 0;
	}

	if (ImGui_ImplWin32_WndProcHandler(_hwnd, _msg, _wparam, _lparam)) {
		return 1;
	}

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}

//�E�B���h�E����
void Application::CreateWindows() {
	//�E�B���h�E�N���X�̐����A�o�^
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = crassName;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&windowClass);

	RECT wrc = { 0,0,window_width,window_height };
	//�֐����g���ăE�B���h�E�̃T�C�Y��␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(windowClass.lpszClassName,//�N���X���w��
		crassName,	//�^�C�g���o�[�̕���
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



void Application::Processing() {
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

void Application::InitFps()
{
	QueryPerformanceFrequency(&Freq);
	//���̎��Ԃ��擾
	QueryPerformanceCounter(&StartTime);
}

void Application::CalculationFps()
{
	//FPS�̌v�Z
	if (iCount == 60) {
		QueryPerformanceCounter(&NowTime);
		fps = 1000.0f / ((static_cast<double>(NowTime.QuadPart
			- StartTime.QuadPart) * 1000 / Freq.QuadPart) / 60);
		StartTime = NowTime;
	}
	iCount++;
}

void Application::CalculationSleep()
{
	QueryPerformanceCounter(&NowTime);

	SleepTime = static_cast<DWORD>((1000.0f / 60.0f)
		* iCount - (NowTime.QuadPart - StartTime.QuadPart)
		* 1000.0f / Freq.QuadPart);

	if (SleepTime > 0 && SleepTime < 18) {
		timeBeginPeriod(1);
		Sleep(SleepTime);
		timeEndPeriod(1);
	}
	else {
		timeBeginPeriod(1);
		Sleep(1);
		timeEndPeriod(1);
	}
}

Application& Application::Instance() {
	static Application instance;
	return instance;
}

bool Application::processMessage()
{
	//���b�Z�[�W���[�v
	MSG msg = {};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//�A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}
