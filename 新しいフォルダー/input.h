#pragma once
#include <dinput.h>
#include <Windows.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace std;
using namespace Microsoft::WRL;

class Input {
private:
	ComPtr<IDirectInput8> dinput = nullptr;
	ComPtr<IDirectInputDevice8> devkeyboard = nullptr;

	BYTE key[256] = {};
	BYTE oldKey[256] = {};

public:
	//èâä˙âª
	bool Initalize(WNDCLASSEX windows, HWND _hwnd);

	//çXêV
	void Update();

	bool Push(BYTE keyNumber);
};