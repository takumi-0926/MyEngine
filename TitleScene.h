#pragma once
#include <DirectXMath.h>
#include <memory>

class Sprite;

enum LoadMode {
	No,
	Start,
	Run,
	End,
};

using namespace std;
using namespace DirectX;

/// <summary>
/// 
/// </summary>
class TitleScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	void ShadowDraw();

	void asyncLoad();

	void loading();

private:
	//ポーズ / ロード
	unique_ptr<Sprite> Pose = nullptr;
	unique_ptr<Sprite> Now_Loading[11] = {};
	unique_ptr<Sprite> LoadControll = nullptr;
	bool pose = false;//ポーズフラグ
	bool load = false;//ローディング
	int _loadMode = 0;//ローディング状態

	bool keyFlag = false;

};

