#pragma once
#include <DirectXMath.h>
#include <memory>

#include "UIManager.h"
#include "StageManager.h"

class Sprite;
class Camera;

using namespace std;
using namespace DirectX;

/// <summary>
/// 
/// </summary>
class TitleScene
{
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(Wrapper* dx12);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();
	void ShadowDraw();

	void asyncLoad();

	void loading();

private:
	Wrapper* dx12;		 //DirectX

	Camera* titleCamera = nullptr;//�^�C�g���J����

	Light* light = nullptr;

	//�^�C�g������ϐ�
	unique_ptr<Sprite> TitleResources[2] = {};
	unique_ptr<Sprite> TitleResources_Start[3] = {};
	unique_ptr<Sprite> TitleResources_Option[3] = {};
	unique_ptr<Sprite> Title = nullptr;
	int TitleWave = 0;
	int TitleHierarchy = 0;
	bool titleStart = 0;
	bool titleOption = 0;

	//���[�h
	unique_ptr<Sprite> Now_Loading[11] = {};
	unique_ptr<Sprite> LoadControll = nullptr;
	bool load = false;//���[�f�B���O
	int _loadMode = 0;//���[�f�B���O���

	bool keyFlag = false;

};

