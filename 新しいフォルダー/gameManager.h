#pragma once
#include "input.h"
#include "audio.h"
#include "dx12Wrapper.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

class GameManager {
private:
	Input* input;
	Audio* audio;
	Wrapper* dx12;

public:
	////コンストラクタ
	//GameManager();

	////デストラクタ
	//~GameManager();

	////初期化
	//void Initalize(Wrapper* dx12,Audio* audio,Input* input);

	////更新
	//void Update();

	////描画
	//void Draw();
};