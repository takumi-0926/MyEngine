#pragma once

class Wrapper;
class DebugText;
class Application;
class PostEffect;
class GameManager;

class HundredBeast
{
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �I��
	/// </summary>
	void Finalize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();


	bool IsEndReqest() { return endReqest; }

private:
	bool endReqest = false;

	Wrapper* dx12 = nullptr;		 //DirectX
	DebugText* debugText = nullptr; //�f�o�b�O�e�L�X�g
	Application* app = nullptr; //�A�v���P�[�V����
	PostEffect* postEffect = nullptr; //�|�X�g�G�t�F�N�g

	GameManager* gameScene = nullptr;
};

