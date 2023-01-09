#pragma once
#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include <dinput.h>
#include "application.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
// CheckHitKeyAll で調べる入力タイプ
//#define CHECKINPUT_KEY   (0x01) //キー入力を調べる
//#define CHECKINPUT_PAD   (0x02) //パッド入力を調べる
//#define CHECKINPUT_MOUSE (0x04) //マウスボタン入力を調べる
//#define CHECKINPUT_ALL   (CHECKINPUT_KEY | CHECKINPUT_PAD | CHECKINPUT_MOUSE) //すべての入力を調べる

class DirectInput final
{
public:
	/** @brief キーボタンの種類 */
	enum ButtonKind
	{
		UpButton,
		DownButton,
		LeftButton,
		RightButton,
		ButtonA,
		ButtonB,
		ButtonX,
		ButtonY,
		ButtonLB,
		ButtonRB,
		ButtonSelect,
		ButtonPouse,
		Button09,
		Button10,
		ButtonKindMax,
	};

	enum ButtonState
	{
		ButtonStateNone,
		ButtonStateDown,
		ButtonStatePush,
		ButtonStateUp,
		ButtonStateMax,
	};

	//ゲームパッドデバイスの作成-デバイス列挙の結果を受け取る構造体
	struct DeviceEnumParameter
	{
		LPDIRECTINPUTDEVICE8* GamePadDevice;
		int FindCount;
	};
private:
	// コンストラクタをprivateにする
	DirectInput() {};
	// デストラクタをprivateにする
	~DirectInput() {}
public:
	// コピーコンストラクタを無効にする
	DirectInput(const DirectInput& obj) = delete;
	// 代入演算子を無効にする
	const DirectInput& operator=(const DirectInput& obj) = delete;
	// インスタンスの取得
	static DirectInput* GetInstance();

	static HWND hwnd;

private: // 静的メンバ変数
	//static BYTE key[256];
	static IDirectInput8* dinput;
	//static IDirectInputDevice8 *devkeyborad;

	static ButtonState g_ButtonStates[ButtonKind::ButtonKindMax];
	static LPDIRECTINPUT8 g_InputInterface;							//!< DIRECTINPUT8のポインタ
	static LPDIRECTINPUTDEVICE8 g_GamePadDevice;					//!< DIRECTINPUTDEVICE8のポインタ

	static DIJOYSTATE pad_data;

	static const int unresponsive_range = 200;

public: // メンバ関数
	// 入力デバイスの初期化
	int InputInit(HINSTANCE hInstance, HWND hWnd);
	// 特定のキーボードの入力状態の取得
	//bool CheckHitKey(int keyCode);
	//// 全ての入力デバイスの状態の取得
	//int CheckHitKeyAll(int CheckType = CHECKINPUT_ALL);
	//// キーボードデバイスの入力状態の取得
	//int GetHitKeyStateAll(char *KeyStateArray);

	/**
	* @brief Input機能終了関数@n
	* Input機能を終了させる@n
	* ※EndEngineで実行されるので開発側が実行する必要はない
	*/
	void ReleaseInput();
	/**
	* @brief 入力情報の更新@n
	* デバイスの入力情報の更新を行う@n
	* 毎フレームに１度必ず実行する必要がある
	*/
	void UpdateInput();
	/**
	* @brief ボタンが押されているかの判定関数
	* @retval true：押されている
	* @retval false：押されていない
	* @param[in] button 判定したいボタンの種類
	*/
	bool IsButtonPush(ButtonKind button);
	/**
	* @brief ボタンが押された瞬間の判定関数
	* @retval true：押した瞬間
	* @retval false：押した瞬間ではない
	* @param[in] button 判定したいボタンの種類
	*/
	bool IsButtonUp(ButtonKind button);
	/**
	* @brief ボタンを離した瞬間の判定関数
	* @retval true：離した瞬間
	* @retval false：瞬間ではない
	* @param[in] button 判定したいボタンの種類
	*/
	bool IsButtonDown(ButtonKind button);
	// 入力インターフェースの作成
	bool CreateInputInterface();
	// ゲームパッドデバイスの作成
	bool CreateGamePadDevice();
	// ゲームパッドの更新
	void UpdateGamePad();

	static BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
	static BOOL StartGamePadControl();
	static BOOL SetUpGamePadProperty(LPDIRECTINPUTDEVICE8 device);
	static BOOL SetUpCooperativeLevel(LPDIRECTINPUTDEVICE8 device);

	static float leftStickX();
	static float leftStickY();

	static float rightStickX();
	static float rightStickY();

	static float rightTriggerZ();

private:
	// キーボードデバイスの更新
	//int KeyInputUpdate();

public:
	static float getLeftX();
	static float getLeftY();

	static float getRightX();
	static float getRightY();
	static float getTriggerZ();

};
