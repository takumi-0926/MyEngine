#pragma once

class Wrapper;
class SceneManager;

class BsScene
{
public:
	BsScene(SceneManager* sceneManager_);

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Wrapper* dx12) = 0;

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;
	virtual void ShadowDraw() = 0;

private:
	SceneManager* sceneManager = nullptr;
};

