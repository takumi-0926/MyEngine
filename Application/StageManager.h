#pragma once
#include <map>
#include <vector>
#include "JsonLoader.h"

#include "stage.h"

using namespace std;
using namespace DirectX;

class Model;
class Object3Ds;

enum GameLocation {
	TitleStage = 0,
	BaseCamp,
	BaseStage,
};

class StageManager
{
public:
	StageManager() = default;
	~StageManager() {};

	static StageManager* GetInstance();

private:
	StageManager(const StageManager& manager) = delete;
	StageManager& operator=(const StageManager& heap) = delete;

public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();
	void ShadowDraw();

private:
	//�X�e�[�W
	int UseStage = 0;//�Q�[�����̃X�e�[�W���ʗp�ϐ�
	map<string, Model*> stageModels;//�X�e�[�W�Ŏg�p���郂�f���̊i�[��
	vector<Stage*>		titleStages;//�X�e�[�W���
	vector<Stage*>			 stages;//�X�e�[�W���
	vector<Stage*>		   baseCamp;//�x�[�X�L�����v���
	JsonData* stageData;//�X�e�[�W�\���ۑ��p
	Object3Ds* skyDome = nullptr;//�w�i�I�u�W�F�N�g
	Model* skyDomeModel = nullptr;//�w�i���f��
public:
	int GetStage() { return UseStage; }
	Stage* GetBaseCamp(int num) { return baseCamp[num]; }
	Object3Ds* GetSkydome() { return skyDome;}
	void SetUseStage(int num) { this->UseStage = num; }
};

