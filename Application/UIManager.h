#pragma once
#include <memory>
#include "SceneEffect/Fade.h"

using namespace std;

const int GATE_MAX = 10;

enum SpriteName {
	Title_UI = 20,		//
	Title_UI_High,		//
	Title_UI_Low,		//
	Start_TItle_UI,		//
	Option_TItle_UI,	//
	Numbers,
	Start_UI_01,
	Start_UI_02,
	Start_UI_03,
};

class Sprite;
/// <summary>
/// UI�Ǘ��N���X�i�V���O���g���j
/// </summary>
class UIManager
{
public:
	UIManager() = default;
	~UIManager() {};

	static UIManager* GetInstance();

private:
	UIManager(const UIManager& manager) = delete;
	UIManager& operator=(const UIManager& heap) = delete;

public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize();
	/// <summary>
	/// �X�V
	/// </summary>
	void TitleUpdate();
	void GameUpdate();
	void EndUpdate();
	void LoadUpdate();
	/// <summary>
	/// �`��
	/// </summary>
	void TitleDarw();
	void GameDarw();
	void ENdDarw();
	void LoadDraw();

private://���ʕϐ��֌W�ϐ�
	int TitleWave = 0;
	int TitleHierarchy = 0;
	bool titleStart = 0;
	bool titleOption = 0;

public://���ʕϐ��֌W�Q�b�^�[
	int GetWave() { return TitleWave; }
	int GetHieraruchy() { return TitleHierarchy; }
	bool GetStart() { return titleStart; }
	bool GetOption() { return titleOption; }

	void SetWave(int wave) { this->TitleWave = wave; }
	void SetHieraruchy(int hierarchy) { this->TitleHierarchy = hierarchy; }
	void SetStart(bool flag) { this->titleStart = flag; }
	void SetOption(bool flag) { this->titleOption = flag; }

private:
	//���UI
	unique_ptr<Sprite> screenUI[6];
	Sprite* weaponSelect = nullptr;
	Sprite* weaponSlot[3] = {};
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

	//�^�C�g������ϐ�
	unique_ptr<Sprite> TitleResources[2] = {};
	unique_ptr<Sprite> TitleResources_Start[3] = {};
	unique_ptr<Sprite> TitleResources_Option[3] = {};
	unique_ptr<Sprite> Title = nullptr;

	//�e�N�X�`���G�t�F�N�g
	unique_ptr<Fade> Effect[8] = {};
	unique_ptr<Fade> fade = nullptr;//�V�[���؂�ւ���
	unique_ptr<Fade> clear = nullptr;//�N���A��
	unique_ptr<Fade> failed = nullptr;//�Q�[���I�[�o�[��
	unique_ptr<Fade> start = nullptr;//�X�^�[�g��
	unique_ptr<Fade> gateBreak_red = nullptr;//��ϋv�lUI�i�� : ��΂���j
	unique_ptr<Fade> gateBreak_yellow = nullptr;//��ϋv�lUI�i�� : �C��t���āj
	unique_ptr<Fade> gateBreak_green = nullptr;//��ϋv�lUI�i�� : ���v�j
	int gateHP = GATE_MAX;//��ϋv�l

	//�|�[�Y / ���[�h
	unique_ptr<Sprite> Pose = nullptr;
	unique_ptr<Sprite> Now_Loading[11] = {};
	unique_ptr<Sprite> LoadControll = nullptr;

	unique_ptr<Sprite> BreakCount[20] = {};
	unique_ptr<Sprite> BreakCountMax[20] = {};
	unique_ptr<Sprite> cross = nullptr;

public:
	Fade* GetFade() { return fade.get(); }
};

