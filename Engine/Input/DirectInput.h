#pragma once
#define DIRECTINPUT_VERSION 0x0800 //DirectInput�̃o�[�W�����w��
#include <dinput.h>
#include "application.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
// CheckHitKeyAll �Œ��ׂ���̓^�C�v
//#define CHECKINPUT_KEY   (0x01) //�L�[���͂𒲂ׂ�
//#define CHECKINPUT_PAD   (0x02) //�p�b�h���͂𒲂ׂ�
//#define CHECKINPUT_MOUSE (0x04) //�}�E�X�{�^�����͂𒲂ׂ�
//#define CHECKINPUT_ALL   (CHECKINPUT_KEY | CHECKINPUT_PAD | CHECKINPUT_MOUSE) //���ׂĂ̓��͂𒲂ׂ�

class DirectInput final
{
public:
	/** @brief �L�[�{�^���̎�� */
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

	//�Q�[���p�b�h�f�o�C�X�̍쐬-�f�o�C�X�񋓂̌��ʂ��󂯎��\����
	struct DeviceEnumParameter
	{
		LPDIRECTINPUTDEVICE8* GamePadDevice;
		int FindCount;
	};
private:
	// �R���X�g���N�^��private�ɂ���
	DirectInput() {};
	// �f�X�g���N�^��private�ɂ���
	~DirectInput() {}
public:
	// �R�s�[�R���X�g���N�^�𖳌��ɂ���
	DirectInput(const DirectInput& obj) = delete;
	// ������Z�q�𖳌��ɂ���
	const DirectInput& operator=(const DirectInput& obj) = delete;
	// �C���X�^���X�̎擾
	static DirectInput* GetInstance();

	static HWND hwnd;

private: // �ÓI�����o�ϐ�
	//static BYTE key[256];
	static IDirectInput8* dinput;
	//static IDirectInputDevice8 *devkeyborad;

	static ButtonState g_ButtonStates[ButtonKind::ButtonKindMax];
	static LPDIRECTINPUT8 g_InputInterface;							//!< DIRECTINPUT8�̃|�C���^
	static LPDIRECTINPUTDEVICE8 g_GamePadDevice;					//!< DIRECTINPUTDEVICE8�̃|�C���^

	static DIJOYSTATE pad_data;

	static const int unresponsive_range = 200;

public: // �����o�֐�
	// ���̓f�o�C�X�̏�����
	int InputInit(HINSTANCE hInstance, HWND hWnd);
	// ����̃L�[�{�[�h�̓��͏�Ԃ̎擾
	//bool CheckHitKey(int keyCode);
	//// �S�Ă̓��̓f�o�C�X�̏�Ԃ̎擾
	//int CheckHitKeyAll(int CheckType = CHECKINPUT_ALL);
	//// �L�[�{�[�h�f�o�C�X�̓��͏�Ԃ̎擾
	//int GetHitKeyStateAll(char *KeyStateArray);

	/**
	* @brief Input�@�\�I���֐�@n
	* Input�@�\���I��������@n
	* ��EndEngine�Ŏ��s�����̂ŊJ���������s����K�v�͂Ȃ�
	*/
	void ReleaseInput();
	/**
	* @brief ���͏��̍X�V@n
	* �f�o�C�X�̓��͏��̍X�V���s��@n
	* ���t���[���ɂP�x�K�����s����K�v������
	*/
	void UpdateInput();
	/**
	* @brief �{�^����������Ă��邩�̔���֐�
	* @retval true�F������Ă���
	* @retval false�F������Ă��Ȃ�
	* @param[in] button ���肵�����{�^���̎��
	*/
	bool IsButtonPush(ButtonKind button);
	/**
	* @brief �{�^���������ꂽ�u�Ԃ̔���֐�
	* @retval true�F�������u��
	* @retval false�F�������u�Ԃł͂Ȃ�
	* @param[in] button ���肵�����{�^���̎��
	*/
	bool IsButtonUp(ButtonKind button);
	/**
	* @brief �{�^���𗣂����u�Ԃ̔���֐�
	* @retval true�F�������u��
	* @retval false�F�u�Ԃł͂Ȃ�
	* @param[in] button ���肵�����{�^���̎��
	*/
	bool IsButtonDown(ButtonKind button);
	// ���̓C���^�[�t�F�[�X�̍쐬
	bool CreateInputInterface();
	// �Q�[���p�b�h�f�o�C�X�̍쐬
	bool CreateGamePadDevice();
	// �Q�[���p�b�h�̍X�V
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
	// �L�[�{�[�h�f�o�C�X�̍X�V
	//int KeyInputUpdate();

public:
	static float getLeftX();
	static float getLeftY();

	static float getRightX();
	static float getRightY();
	static float getTriggerZ();

};
