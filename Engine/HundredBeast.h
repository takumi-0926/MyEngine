#pragma once
#include "Framework.h"
#include "BsScene.h"

class PostEffect;
class TitleScene;
class PlayScene;
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
};

