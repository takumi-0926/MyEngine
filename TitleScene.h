#pragma once
#include <DirectXMath.h>
#include <memory>

#include "UIManager.h"

class Sprite;
class Camera;

enum LoadMode {
	No,
	Start,
	Run,
	End,
};

using namespace std;
using namespace DirectX;

/// <summary>
/// 
/// </summary>
class TitleScene
{
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

	void asyncLoad();

	void loading();

private:
	Camera* titleCamera = nullptr;//タイトルカメラ

	//タイトル周り変数
	unique_ptr<Sprite> TitleResources[2] = {};
	unique_ptr<Sprite> TitleResources_Start[3] = {};
	unique_ptr<Sprite> TitleResources_Option[3] = {};
	unique_ptr<Sprite> Title = nullptr;
	int TitleWave = 0;
	int TitleHierarchy = 0;
	bool titleStart = 0;
	bool titleOption = 0;

	//ロード
	unique_ptr<Sprite> Now_Loading[11] = {};
	unique_ptr<Sprite> LoadControll = nullptr;
	bool load = false;//ローディング
	int _loadMode = 0;//ローディング状態

	bool keyFlag = false;

};

