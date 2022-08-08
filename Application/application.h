#pragma once
#include <Windows.h>
#include "dx12Wrapper.h"

class Application {
private:
	WNDCLASSEX windowClass;
	HWND hwnd;

	//�E�B���h�E����
	void CreateWindows(HWND& hwnd, WNDCLASSEX& windowClass);
public://�ÓI�����o�֐�
	static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//�E�B���h�E�萔
	static const int window_width = 1280;// ����
	static const int window_height = 720;// �c��
public:

	Application();

	///Application�̃V���O���g���C���X�^���X�𓾂�
	static Application& Instance();

	//������
	bool Init();

	//���[�v
	void Run();

	//�㏈��
	void Processing();


	//�E�B���h�E�T�C�Y�̎擾
	SIZE GetWindowSize()const;

	HWND _hwnd();
	WNDCLASSEX _windowClass();
};
