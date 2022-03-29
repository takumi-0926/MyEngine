#include "input.h"
#include <cassert>

bool Input::Initalize(WNDCLASSEX windows, HWND _hwnd)
{
	auto result = DirectInput8Create(windows.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	assert(SUCCEEDED(result));

	//GUID_Joystick,GUID_SysMouse�ŕʂ̃f�o�C�X���g����

	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	result = devkeyboard->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	return true;
}

void Input::Update()
{
	devkeyboard->Acquire();
	//devkeyboard->Poll();

	//�L�[�̃g���K�[�����擾
	memcpy(oldKey, key, sizeof(key));

	//�L�[�̓���
	devkeyboard->GetDeviceState(sizeof(key), key);
}

bool Input::Push(BYTE keyNumber) {
	// �ُ�Ȉ��������o
	assert(0 <= keyNumber && keyNumber <= 256);

	if (key[keyNumber]) {
		return true;
	}
	return false;
}
