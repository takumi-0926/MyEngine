#pragma once
#include <DirectXMath.h>
#include <memory>
#include "BsScene.h"

#include "UIManager.h"
#include "StageManager.h"

class Sprite;
class Camera;

using namespace std;
using namespace DirectX;

/// <summary>
/// 
/// </summary>
class TitleScene : public BsScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Wrapper* _dx12) override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
	void ShadowDraw() override;

	void asyncLoad();

	void loading();

private:
	Wrapper* dx12;		 //DirectX

	Camera* titleCamera = nullptr;//タイトルカメラ

	Light* light = nullptr;

	//タイトル周り変数
	unique_ptr<Sprite> TitleResources[2] = {};
	unique_ptr<Sprite> TitleResources_Start[3] = {};
	unique_ptr<Sprite> TitleResources_Option[3] = {};
	unique_ptr<Sprite> Title = nullptr;
	int TitleWave = 0;
	int TitleHierarchy = 0;
	bool titleStart = 0;
	bool titleOption = 0;

	bool keyFlag = false;

};

