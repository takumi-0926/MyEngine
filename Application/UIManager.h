#pragma once
#include <memory>

using namespace std;

const int GATE_MAX = 10;

enum SpriteName {
	Title_UI = 19,		//
	Title_UI_High,		//
	Title_UI_Low,		//
	Start_TItle_UI,		//
	Option_TItle_UI,	//
	Numbers,
	Start_UI_01,
	Start_UI_02,
	Start_UI_03,
};

class Fade;
class Sprite;
/// <summary>
/// UI�Ǘ��N���X�i�V���O���g���j
/// </summary>
class UIManager
{
public:
	UIManager() = default;
	~UIManager() {};

private:
	UIManager(const UIManager& manager) = delete;
	UIManager& operator=(const UIManager& heap) = delete;

public:
	void Initialize();
	void Update();
	void Darw();

private:
	//�e�N�X�`���G�t�F�N�g
	unique_ptr<Fade> Effect[8];
	Fade* fade = nullptr;//�V�[���؂�ւ���
	Fade* clear = nullptr;//�N���A��
	Fade* failed = nullptr;//�Q�[���I�[�o�[��
	Fade* start = nullptr;//�X�^�[�g��
	Fade* gateBreak_red = nullptr;//��ϋv�lUI�i�� : ��΂���j
	Fade* gateBreak_yellow = nullptr;//��ϋv�lUI�i�� : �C��t���āj
	Fade* gateBreak_green = nullptr;//��ϋv�lUI�i�� : ���v�j

	//���UI
	unique_ptr<Sprite> screenUI[6];
	Sprite* weaponSelect = nullptr;
	Sprite* weaponSlot[3] = {};
	Sprite* Title = nullptr;
	Sprite* End = nullptr;
	Sprite* hp = nullptr;
	Sprite* HpBer = nullptr;
	bool result = false;//�N���A����

	//�J�E���gUI����ϐ�
	unique_ptr<Sprite> One_Numbers[10] = {};//0�`9�̐����X�v���C�g
	unique_ptr<Sprite> Ten_Numbers[10] = {};//0�`9�̐����X�v���C�g
	unique_ptr<Sprite> Start_UI_01 = {};
	unique_ptr<Sprite> Start_UI_02 = {};
	unique_ptr<Fade> Start_UI_03 = {};
	float calculationTime = 60.0f;//�v�Z�p
	int startTime = 0;//�J�n�J�E���g
	int one_place = 0;//��̈�
	int tens_place = 0;//�\�̈�

	unique_ptr<Sprite> BreakCount[20] = {};
	unique_ptr<Sprite> BreakCountMax[20] = {};
	unique_ptr<Sprite> cross = nullptr;

};

