#include "DirectInput.h"
#include <DirectXmath.h>


IDirectInput8 *DirectInput::dinput = nullptr;
//IDirectInputDevice8 *DirectInput::devkeyborad = nullptr;
HWND DirectInput::hwnd = {};
DirectInput::ButtonState DirectInput::g_ButtonStates[ButtonKind::ButtonKindMax] = {};
LPDIRECTINPUT8 DirectInput::g_InputInterface = nullptr;
LPDIRECTINPUTDEVICE8 DirectInput::g_GamePadDevice = nullptr;
//BYTE DirectInput::key[256] = {};
DIJOYSTATE DirectInput::pad_data;

using  namespace DirectX;

DirectInput *DirectInput::GetInstance()
{
	// インスタンスの生成
	static DirectInput instance;

	return &instance;
}
//LPDIRECTINPUTDEVICE8    lpKeyboard = NULL;             // キーボードデバイス


int DirectInput::InputInit(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;
	hwnd = hWnd;

	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&dinput, nullptr);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		OutputDebugStringA("DirectInput8の作成に失敗しました\n");
#endif // _DEBUG

		return -1;
	}

	// キーボードデバイスの生成
	//hr = dinput->CreateDevice(GUID_SysKeyboard, &devkeyborad, NULL);
	//if (FAILED(hr))
	//{
	//	return -1;
	//}

	//// 入力データ形式のセット
	//hr = devkeyborad->SetDataFormat(&c_dfDIKeyboard); //標準形式
	//if (FAILED(hr))
	//{
	//	return -1;
	//}

	//// 排他制御レベルのセット
	//hr = devkeyborad->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//if (FAILED(hr))
	//{
	//	return -1;
	//}

	// インターフェース作成
	if (CreateInputInterface() == false)
	{
		return false;
	}

	// デバイス作成
	if (CreateGamePadDevice() == false)
	{
		ReleaseInput();
		return false;
	}

	// 入力情報の初期化
	for (int i = 0; i < ButtonKind::ButtonKindMax; i++)
	{
		g_ButtonStates[i] = ButtonState::ButtonStateNone;
	}
	return true;
}


void DirectInput::ReleaseInput()
{
	// デバイスの解放
	if (g_GamePadDevice != nullptr)
	{
		// 制御を停止
		g_GamePadDevice->Unacquire();
		g_GamePadDevice->Release();
		g_GamePadDevice = nullptr;
	}

	// インターフェースの解放
	if (g_InputInterface != nullptr)
	{
		g_InputInterface->Release();
		g_InputInterface = nullptr;
	}
}

void DirectInput::UpdateInput()
{
	UpdateGamePad();
}

bool DirectInput::IsButtonPush(ButtonKind button)
{
	if (g_ButtonStates[button] == ButtonState::ButtonStatePush)
	{
		return true;
	}

	return false;
}

bool DirectInput::IsButtonUp(ButtonKind button)
{
	if (g_ButtonStates[button] == ButtonState::ButtonStateUp)
	{
		return true;
	}

	return false;
}

bool DirectInput::IsButtonDown(ButtonKind button)
{
	if (g_ButtonStates[button] == ButtonState::ButtonStateDown)
	{
		return true;
	}

	return false;
}

bool DirectInput::CreateInputInterface()
{
	// インターフェース作成
	HRESULT ret = DirectInput8Create(
		GetModuleHandle(nullptr),	// インスタンスハンドル
		DIRECTINPUT_VERSION,		// DirectInputのバージョン
		IID_IDirectInput8,			// 使用する機能
		(void **)&g_InputInterface,	// 作成されたインターフェース代入用
		NULL						// NULL固定
	);

	if (FAILED(ret))
	{
		return false;
	}

	return true;
}

bool DirectInput::CreateGamePadDevice()
{
	DeviceEnumParameter parameter;

	parameter.FindCount = 0;
	parameter.GamePadDevice = &g_GamePadDevice;

	// GAMEPADを調べる
	g_InputInterface->EnumDevices(
		DI8DEVTYPE_GAMEPAD,			// 検索するデバイスの種類
		DeviceFindCallBack,			// 発見時に実行する関数
		&parameter,					// 関数に渡す値
		DIEDFL_ATTACHEDONLY			// 検索方法
	);

	// JOYSTICKを調べる
	g_InputInterface->EnumDevices(
		DI8DEVTYPE_JOYSTICK,
		DeviceFindCallBack,
		&parameter,
		DIEDFL_ATTACHEDONLY
	);

	// どちらも見つけることが出来なかったら失敗
	if (parameter.FindCount == 0)
	{
		return false;
	}

	int count = 0;
	// 制御開始
	while (StartGamePadControl() == false)
	{
		Sleep(100);
		count++;
		if (count >= 5)
		{
			break;
		}
	}

	return true;
}

void DirectInput::UpdateGamePad()
{
	if (g_GamePadDevice == nullptr)
	{
		return;
	}

	// デバイス取得
	HRESULT hr = g_GamePadDevice->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
	if (FAILED(hr))
	{
		// 再度制御開始
		if (FAILED(g_GamePadDevice->Acquire()))
		{
			for (int i = 0; i < ButtonKind::ButtonKindMax; i++)
			{
				g_ButtonStates[i] = ButtonState::ButtonStateNone;
			}
			g_GamePadDevice->Poll();
		}
		return;
	}

	bool is_push[ButtonKind::ButtonKindMax] = {false};
	// スティック判定
	//int unresponsive_range = 200;
	if (pad_data.lX < -unresponsive_range)
	{
		is_push[ButtonKind::LeftButton] = true;
	}
	else if (pad_data.lX > unresponsive_range)
	{
		is_push[ButtonKind::RightButton] = true;
	}

	if (pad_data.lY < -unresponsive_range)
	{
		is_push[ButtonKind::UpButton] = true;
	}
	else if (pad_data.lY > unresponsive_range)
	{
		is_push[ButtonKind::DownButton] = true;
	}

	// 十字キー判定
	if (pad_data.rgdwPOV[0] != 0xFFFFFFFF)
	{
		float rad = XMConvertToRadians((pad_data.rgdwPOV[0] / 100.0f));
		// 本来はxがcos、yがsinだけど、rgdwPOVは0が上から始まるので、
		// cosとsinを逆にした方が都合がいい
		float x = sinf(rad);
		float y = cosf(rad);

		if (x < -0.01f)
		{
			is_push[ButtonKind::LeftButton] = true;
		}
		else if (x > 0.01f)
		{
			is_push[ButtonKind::RightButton] = true;
		}

		if (y > 0.01f)
		{
			is_push[ButtonKind::UpButton] = true;
		}
		else if (y < -0.01f)
		{
			is_push[ButtonKind::DownButton] = true;
		}
	}

	// ボタン判定
	for (int i = 0; i < 32; i++)
	{
		if (!(pad_data.rgbButtons[i] & 0x80))
		{
			continue;
		}

		switch (i)
		{
		case 0:
			is_push[ButtonKind::ButtonA] = true;
			break;
		case 1:
			is_push[ButtonKind::ButtonB] = true;
			break;
		case 2:
			is_push[ButtonKind::ButtonX] = true;
			break;
		case 3:
			is_push[ButtonKind::ButtonY] = true;
			break;
		case 4:
			is_push[ButtonKind::ButtonLB] = true;
			break;
		case 5:
			is_push[ButtonKind::ButtonRB] = true;
			break;
		case 6:
			is_push[ButtonKind::ButtonSelect] = true;
			break;
		case 7:
			is_push[ButtonKind::ButtonPouse] = true;
			break;
		case 8:
			is_push[ButtonKind::Button09] = true;
			break;
		case 9:
			is_push[ButtonKind::Button10] = true;
			break;
		}
	}

	// 入力情報からボタンの状態を更新する
	for (int i = 0; i < ButtonKind::ButtonKindMax; i++)
	{
		if (is_push[i] == true)
		{
			if (g_ButtonStates[i] == ButtonState::ButtonStateNone)
			{
				g_ButtonStates[i] = ButtonState::ButtonStatePush;
			}
			else
			{
				g_ButtonStates[i] = ButtonState::ButtonStateDown;
			}
		}
		else
		{
			if (g_ButtonStates[i] == ButtonState::ButtonStatePush)
			{
				g_ButtonStates[i] = ButtonState::ButtonStateUp;
			}
			else
			{
				g_ButtonStates[i] = ButtonState::ButtonStateNone;
			}
		}
	}
}

BOOL DirectInput::DeviceFindCallBack(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	DeviceEnumParameter *parameter = (DeviceEnumParameter *)pvRef;
	LPDIRECTINPUTDEVICE8 device = nullptr;

	// 既に発見しているなら終了
	if (parameter->FindCount >= 1)
	{
		return DIENUM_STOP;
	}

	// デバイス生成
	HRESULT hr = g_InputInterface->CreateDevice(
		lpddi->guidInstance,
		parameter->GamePadDevice,
		NULL);

	if (FAILED(hr))
	{
		return DIENUM_STOP;
	}

	// 入力フォーマットの指定
	device = *parameter->GamePadDevice;
	hr = device->SetDataFormat(&c_dfDIJoystick);

	if (FAILED(hr))
	{
		return DIENUM_STOP;
	}

	// プロパティの設定
	if (SetUpGamePadProperty(device) == false)
	{
		return DIENUM_STOP;
	}

	// 協調レベルの設定
	if (SetUpCooperativeLevel(device) == false)
	{
		return DIENUM_STOP;
	}

	// 発見数をカウント
	parameter->FindCount++;

	return DIENUM_CONTINUE;
}

BOOL DirectInput::StartGamePadControl()
{
	// デバイスが生成されてない
	if (g_GamePadDevice == nullptr)
	{
		return false;
	}

	// 制御開始
	if (FAILED(g_GamePadDevice->Acquire()))
	{
		return false;
	}

	DIDEVCAPS cap;
	g_GamePadDevice->GetCapabilities(&cap);
	// ポーリング判定
	if (cap.dwFlags & DIDC_POLLEDDATAFORMAT)
	{
		DWORD error = GetLastError();
		// ポーリング開始
		/*
			PollはAcquireの前に行うとされていたが、
			Acquireの前で実行すると失敗したので
			後で実行するようにした
		*/
		if (FAILED(g_GamePadDevice->Poll()))
		{
			return false;
		}
	}

	return true;
}

BOOL DirectInput::SetUpGamePadProperty(LPDIRECTINPUTDEVICE8 device)
{
	// 軸モードを絶対値モードとして設定
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;
	if (FAILED(device->SetProperty(DIPROP_AXISMODE, &diprop.diph)))
	{
		return false;
	}

	// X軸の値の範囲設定
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = -1000;
	diprg.lMax = 1000;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
	{
		return false;
	}

	// Y軸の値の範囲設定
	diprg.diph.dwObj = DIJOFS_Y;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
	{
		return false;
	}

	// Z軸の値の範囲設定
	diprg.diph.dwObj = DIJOFS_Z;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
	{
		return false;
	}

	// RX軸の値の範囲設定
	diprg.diph.dwObj = DIJOFS_RX;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
	{
		return false;
	}

	// RY軸の値の範囲設定
	diprg.diph.dwObj = DIJOFS_RY;
	if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
	{
		return false;
	}

	// RZ軸の値の範囲設定
	diprg.diph.dwObj = DIJOFS_RZ;
	auto hr = device->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))
	{
		return true;
	}

	return true;
}

BOOL DirectInput::SetUpCooperativeLevel(LPDIRECTINPUTDEVICE8 device)
{
	//協調モードの設定
	if (FAILED(device->SetCooperativeLevel(
		hwnd,
		DISCL_EXCLUSIVE | DISCL_FOREGROUND)
	))
	{
		return false;
	}

	return true;
}

float DirectInput::leftStickX()
{
	if (pad_data.lX < -unresponsive_range)
	{
		return pad_data.lX / 1000.0f;
		//is_push[ButtonKind::LeftButton] = true;
	}
	else if (pad_data.lX > unresponsive_range)
	{
		return pad_data.lX / 1000.0f;
		//is_push[ButtonKind::RightButton] = true;
	}

	return 0.0f;
}

float DirectInput::leftStickY()
{
	if (pad_data.lY < -unresponsive_range)
	{
		return pad_data.lY / 1000.0f;
		//is_push[ButtonKind::UpButton] = true;
	}
	else if (pad_data.lY > unresponsive_range)
	{
		return pad_data.lY / 1000.0f;
		//is_push[ButtonKind::DownButton] = true;
	}

	return 0.0f;
}

float DirectInput::rightStickX()
{
	if (pad_data.lRx < -unresponsive_range)
	{
		return pad_data.lRx / 1000.0f;
		//is_push[ButtonKind::LeftButton] = true;
	}
	else if (pad_data.lRx > unresponsive_range)
	{
		return pad_data.lRx / 1000.0f;
		//is_push[ButtonKind::RightButton] = true;
	}

	return 0.0f;
}

float DirectInput::rightStickY()
{
	if (pad_data.lRy < -unresponsive_range)
	{
		return pad_data.lRy / 1000.0f;
		//is_push[ButtonKind::UpButton] = true;
	}
	else if (pad_data.lRy > unresponsive_range)
	{
		return pad_data.lRy / 1000.0f;
		//is_push[ButtonKind::DownButton] = true;
	}

	return 0.0f;
}

float DirectInput::rightTriggerZ()
{
	if (pad_data.lZ < -unresponsive_range)
	{
		return pad_data.lZ / 1000.0f;
		//is_push[ButtonKind::UpButton] = true;
	}
	else if (pad_data.lZ > unresponsive_range)
	{
		return pad_data.lZ / 1000.0f;
		//is_push[ButtonKind::DownButton] = true;
	}

	return 0.0f;
}

float DirectInput::getLeftX()
{
	return pad_data.lX / 1000.0f;
}

float DirectInput::getLeftY()
{
	return pad_data.lY / 1000.0f;
}

float DirectInput::getRightX()
{
	return pad_data.lRx / 1000.0f;
}

float DirectInput::getRightY()
{
	return pad_data.lRy / 1000.0f;
}

float DirectInput::getTriggerZ()
{
	return pad_data.lZ / 1000.0f;
}

//int DirectInput::KeyInputUpdate()
//{
//	HRESULT hr;
//
//	ZeroMemory(key, sizeof(key));
//	hr = devkeyborad->GetDeviceState(sizeof(key), key);
//	if (FAILED(hr)) {
//		// 失敗なら再開させてもう一度取得
//		devkeyborad->Acquire();
//		devkeyborad->GetDeviceState(sizeof(key), key);
//	}
//
//	return 0;
//}
