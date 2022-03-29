#pragma once
#include <dinput.h>
#include <Windows.h>
#include <wrl.h>

#include "application.h"

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace std;
using namespace Microsoft::WRL;

class Input {
public:
	//namespace省略
	//エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	Application* App = nullptr;

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
	//初期化
	bool Initalize(Application* App);

	//更新
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