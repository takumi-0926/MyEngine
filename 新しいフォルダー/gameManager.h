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
	////�R���X�g���N�^
	//GameManager();

	////�f�X�g���N�^
	//~GameManager();

	////������
	//void Initalize(Wrapper* dx12,Audio* audio,Input* input);

	////�X�V
	//void Update();

	////�`��
	//void Draw();
};