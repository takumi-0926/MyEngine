#include "input.h"
#include <cassert>

bool Input::Initalize(Application* App)
{
	this->App = App;

	directInput->InputInit(
		App->_windowClass().hInstance,
		App->_hwnd());//////////////////////////

	//DirectInputのインスタンスを作成
	auto result = DirectInput8Create(App->_windowClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	//キーボードデバイスを作成
	///GUID_Joystick,GUID_SysMouseで別のデバイスも使える
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	assert(SUCCEEDED(result));

	//マウスデバイスの生成
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット（キー）
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット（キー）
	result = devkeyboard->SetCooperativeLevel(App->_hwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//入力データ形式のセット（マウス）
	result = devMouse->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット（マウス）
	result = devMouse->SetCooperativeLevel(App->_hwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	GetCursorPos(&point);
	ScreenToClient(FindWindowA("DX12Sample", nullptr), &point);

	return true;
}

void Input::Update()
{
	HRESULT result;
	{//キー
		directInput->UpdateInput();////////////////////////////////

		//前回のキー入力を保存
		memcpy(oldKey, key, sizeof(key));

		//キーボード情報の取得開始
		devkeyboard->Acquire();
		//devkeyboard->Poll();

		//キーのトリガー情報を取得
		memcpy(oldKey, key, sizeof(key));

		//キーの入力
		result = devkeyboard->GetDeviceState(sizeof(key), key);
	}

	{//マウス
		devMouse->Acquire();

		mouseStatePre = mouseState;

		devMouse->GetDeviceState(sizeof(mouseState), &mouseState);

		mousePos.x = point.x;
		mousePos.y = point.y;
	}
}

bool Input::Push(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	if (key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::Trigger(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	if (key[keyNumber] && !oldKey[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::MouseLeftPush() {
	// 0でなければ押している
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// 押していない
	return false;

}

bool Input::MouseWheelPush(){
	// 0でなければ押している
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// 押していない
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
