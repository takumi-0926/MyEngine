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
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


	bool IsEndReqest() { return endReqest; }

private:
	bool endReqest = false;

	Wrapper* dx12 = nullptr;		 //DirectX
	DebugText* debugText = nullptr; //デバッグテキスト
	Application* app = nullptr; //アプリケーション
	PostEffect* postEffect = nullptr; //ポストエフェクト

	GameManager* gameScene = nullptr;
};

