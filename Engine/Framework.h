#pragma once
class Wrapper;
class DebugText;
class Application;
class PostEffect;

#include "..\SceneManager.h"

class Framework
{
public:
	/// <summary>
	/// ���s
	/// </summary>
	void Run();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// �I��
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw();

	bool IsEndReqest() { return endReqest; }

protected:
	bool endReqest = false;

	Wrapper* dx12 = nullptr;		 //DirectX
	DebugText* debugText = nullptr; //�f�o�b�O�e�L�X�g
	Application* app = nullptr; //�A�v���P�[�V����
	PostEffect* postEffect = nullptr; //�|�X�g�G�t�F�N�g

	SceneManager* sceneManager = nullptr;
};

