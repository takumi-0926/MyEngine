#pragma once
class Wrapper;
class DebugText;
class Application;

class Framework
{
public:
	/// <summary>
	/// 実行
	/// </summary>
	void Run();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	bool IsEndReqest() { return endReqest; }

protected:
	bool endReqest = false;

	Wrapper* dx12 = nullptr;		 //DirectX
	DebugText* debugText = nullptr; //デバッグテキスト
	Application* app = nullptr; //アプリケーション
};

