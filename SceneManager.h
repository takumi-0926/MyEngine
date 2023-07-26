#pragma once
#include "..\BsScene.h"

/// <summary>
/// シーン管理
/// </summary>
class SceneManager
{
public:
	/// <summary>
	/// 
	/// </summary>
	void Update();

	/// <summary>
	/// 
	/// </summary>
	void Draw();
	void ShadowDraw();

	//次シーン設定
	void SetNextScene(BsScene* scene) { nextScene = scene; }
private:
	//シーン
	BsScene* Scene = nullptr;
	//次のシーン
	BsScene* nextScene = nullptr;

};

