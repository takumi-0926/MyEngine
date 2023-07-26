#pragma once
#include "..\BsScene.h"

/// <summary>
/// �V�[���Ǘ�
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

	//���V�[���ݒ�
	void SetNextScene(BsScene* scene) { nextScene = scene; }
private:
	//�V�[��
	BsScene* Scene = nullptr;
	//���̃V�[��
	BsScene* nextScene = nullptr;

};

