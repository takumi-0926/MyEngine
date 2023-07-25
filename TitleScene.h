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
	/// ������
	/// </summary>
	void Initialize();

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
	//�|�[�Y / ���[�h
	unique_ptr<Sprite> Pose = nullptr;
	unique_ptr<Sprite> Now_Loading[11] = {};
	unique_ptr<Sprite> LoadControll = nullptr;
	bool pose = false;//�|�[�Y�t���O
	bool load = false;//���[�f�B���O
	int _loadMode = 0;//���[�f�B���O���

	bool keyFlag = false;

};

