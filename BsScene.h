#pragma once

class Wrapper;
class SceneManager;

class BsScene
{
public:
	BsScene(SceneManager* sceneManager_);

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize(Wrapper* dx12) = 0;

	/// <summary>
	/// �I��
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() = 0;
	virtual void ShadowDraw() = 0;

private:
	SceneManager* sceneManager = nullptr;
};

