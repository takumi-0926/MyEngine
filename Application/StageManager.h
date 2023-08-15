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
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	void ShadowDraw();

private:
	//ステージ
	int UseStage = 0;//ゲーム中のステージ識別用変数
	map<string, Model*> stageModels;//ステージで使用するモデルの格納先
	vector<Stage*>		titleStages;//ステージ情報
	vector<Stage*>			 stages;//ステージ情報
	vector<Stage*>		   baseCamp;//ベースキャンプ情報
	JsonData* stageData;//ステージ構成保存用
	Object3Ds* skyDome = nullptr;//背景オブジェクト
	Model* skyDomeModel = nullptr;//背景モデル
public:
	int GetStage() { return UseStage; }
	Stage* GetBaseCamp(int num) { return baseCamp[num]; }
	Object3Ds* GetSkydome() { return skyDome;}
	void SetUseStage(int num) { this->UseStage = num; }
};

