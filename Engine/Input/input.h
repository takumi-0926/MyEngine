#pragma once
#include <dinput.h>
#include <Windows.h>
#include <wrl.h>

#include "DirectInput.h"
#include "application.h"
#include "..\includes.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace std;
using namespace Microsoft::WRL;

static DirectInput* directInput = nullptr;

class Input {
public:
	//namespace�ȗ�
	//�G�C���A�X
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;

private:
	Application* App = nullptr;

	// private�ȃR���X�g���N�^�i�V���O���g���p�^�[���j
	Input() = default;
	// private�ȃf�X�g���N�^�i�V���O���g���p�^�[���j
	~Input() = default;
	// �R�s�[�R���X�g���N�^���֎~�i�V���O���g���p�^�[���j
	Input(const Input & obj) = delete;
	// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
	void operator=(const Input & obj) = delete;

public:
	struct MouseMove {
		LONG lX;
		LONG lY;
		LONG lZ;
	};
private:
	ComPtr<IDirectInput8> dinput = nullptr;
	ComPtr<IDirectInputDevice8> devkeyboard = nullptr;

	ComPtr<IDirectInputDevice8> devMouse;

	BYTE key[256] = {};
	BYTE oldKey[256] = {};

	POINT point;
	XMFLOAT2 mousePos;

	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};

public:
	static Input* GetInstance();

	//������
	bool Initalize(Application* App);

	//�X�V
	void Update();

	bool Push(BYTE keyNumber);

	bool Trigger(BYTE keyNumber);

	bool MouseLeftPush();

	bool MouseWheelPush();

	MouseMove GetMouseMove();

	XMFLOAT2 GetPos() {
		return mousePos;
	}
};