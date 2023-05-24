#pragma once
#include <Windows.h>
#pragma comment(lib,"winmm.lib")

class Application {
private://�����o�ϐ�
	WNDCLASSEX windowClass{};
	HWND hwnd = nullptr;
	double fps = 0;
	LARGE_INTEGER Freq = { 0 };
	LARGE_INTEGER StartTime = { 0 };
	LARGE_INTEGER NowTime = { 0 };
	int iCount = 0;
	DWORD SleepTime = 0;

public://�ÓI�����o�ϐ�

	//�E�B���h�E�萔
	static const int window_width = 1280;// ����
	static const int window_height = 720;// �c��
	static const wchar_t crassName[];    // �N���X�l�[��
public:

	//�R���X�g���N�^
	Application();


	///Application�̃V���O���g���C���X�^���X�𓾂�
	static Application& Instance();

	static LRESULT WindowProcedure(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

	//�E�B���h�E����
	void CreateWindows();

	/// <summary>
	/// ���b�Z�[�W
	/// </summary>
	/// <returns>�I������</returns>
	bool processMessage();

	//�I��������
	void Processing();

	void InitFps();
	void CalculationFps();
	void CalculationSleep();

	//�Q�b�^�[
	SIZE GetWindowSize()const {
		SIZE winSize;
		winSize.cx = window_width;
		winSize.cy = window_height;
		return winSize;
	};
	HWND _hwnd() { return hwnd; }
	HINSTANCE classInstance() { return windowClass.hInstance; };
};
