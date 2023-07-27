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
	/// ‰Šú‰»
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// I—¹
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// XV
	/// </summary>
	void Update() override;

	/// <summary>
	/// •`‰æ
	/// </summary>
	void Draw() override;

private:
};

