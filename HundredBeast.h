#pragma once
#include "Framework.h"

class PostEffect;
class GameManager;

class HundredBeast : public Framework
{
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// �I��
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

private:
	PostEffect* postEffect = nullptr; //�|�X�g�G�t�F�N�g

	GameManager* gameScene = nullptr;
};

