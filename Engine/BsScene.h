#pragma once

class Wrapper;
class SceneManager;

class BsScene
{
public:
	virtual ~BsScene() = default;

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize(Wrapper* _dx12) = 0;

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

	//
	void SetSceneManager(SceneManager* sceneManager_) { sceneManager = sceneManager_; }
protected:
	SceneManager* sceneManager = nullptr;
};

