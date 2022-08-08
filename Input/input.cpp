#include "input.h"
#include <cassert>

bool Input::Initalize(Application* App)
{
	this->App = App;

	directInput->InputInit(
		App->_windowClass().hInstance,
		App->_hwnd());//////////////////////////

	//DirectInput�̃C���X�^���X���쐬
	auto result = DirectInput8Create(App->_windowClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	//�L�[�{�[�h�f�o�C�X���쐬
	///GUID_Joystick,GUID_SysMouse�ŕʂ̃f�o�C�X���g����
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	assert(SUCCEEDED(result));

	//�}�E�X�f�o�C�X�̐���
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g�i�L�[�j
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g�i�L�[�j
	result = devkeyboard->SetCooperativeLevel(App->_hwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g�i�}�E�X�j
	result = devMouse->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g�i�}�E�X�j
	result = devMouse->SetCooperativeLevel(App->_hwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	GetCursorPos(&point);
	ScreenToClient(FindWindowA("DX12Sample", nullptr), &point);

	return true;
}

void Input::Update()
{
	HRESULT result;
	{//�L�[
		directInput->UpdateInput();////////////////////////////////

		//�O��̃L�[���͂�ۑ�
		memcpy(oldKey, key, sizeof(key));

		//�L�[�{�[�h���̎擾�J�n
		devkeyboard->Acquire();
		//devkeyboard->Poll();

		//�L�[�̃g���K�[�����擾
		memcpy(oldKey, key, sizeof(key));

		//�L�[�̓���
		result = devkeyboard->GetDeviceState(sizeof(key), key);
	}

	{//�}�E�X
		devMouse->Acquire();

		mouseStatePre = mouseState;

		devMouse->GetDeviceState(sizeof(mouseState), &mouseState);

		mousePos.x = point.x;
		mousePos.y = point.y;
	}
}

bool Input::Push(BYTE keyNumber)
{
	// �ُ�Ȉ��������o
	assert(0 <= keyNumber && keyNumber <= 256);

	if (key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::Trigger(BYTE keyNumber)
{
	// �ُ�Ȉ��������o
	assert(0 <= keyNumber && keyNumber <= 256);

	if (key[keyNumber] && !oldKey[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::MouseLeftPush() {
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;

}

bool Input::MouseWheelPush(){
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

Input::MouseMove Input::GetMouseMove()
{
	MouseMove tmp;
	tmp.lX = mouseState.lX;
	tmp.lY = mouseState.lY;
	tmp.lZ = mouseState.lZ;
	return tmp;
}
