#pragma once
#include "BsScene.h"

/// <summary>
/// �V�[���Ǘ�
/// </summary>
class SceneManager
{
public:
	SceneManager(Wrapper* _dx12);

	~SceneManager();

	/// <summary>
	/// 
	/// </summary>
	void Update();

	/// <summary>
	/// 
	/// </summary>
	void Draw();
	void ShadowDraw();

	//���V�[���ݒ�
	void SetNextScene(BsScene* scene) { nextScene = scene; }
private:
	Wrapper* dx12 = nullptr;		 //DirectX
	//�V�[��
	BsScene* Scene = nullptr;
	//���̃V�[��
	BsScene* nextScene = nullptr;

};

