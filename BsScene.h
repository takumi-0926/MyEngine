#pragma once

class Wrapper;
class SceneManager;

class BsScene
{
public:
	BsScene(SceneManager* sceneManager_);

	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	virtual void Initialize(Wrapper* dx12) = 0;

	/// <summary>
	/// I—¹
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// XV
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// •`‰æ
	/// </summary>
	virtual void Draw() = 0;
	virtual void ShadowDraw() = 0;

private:
	SceneManager* sceneManager = nullptr;
};

