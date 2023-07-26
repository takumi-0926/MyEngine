#pragma once
#include "Framework.h"
#include "..\BsScene.h"

class PostEffect;
class TitleScene;
class PlayScene;
class GameManager;

class HundredBeast : public Framework
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

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

private:
};

