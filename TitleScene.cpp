#include "TitleScene.h"
#include "Sprite/sprite.h"
#include <future>

//std::thread th = {};

void TitleScene::Initialize()
{
}

void TitleScene::Update()
{
}

void TitleScene::Draw()
{
}

void TitleScene::asyncLoad()
{
	//auto th = std::thread([&] {LoadGameResources(); });

	//th.join();

	//_loadMode = LoadMode::End;
}

void TitleScene::loading()
{

	//if (load) {
	//	switch (_loadMode)
	//	{
	//	case LoadMode::No:
	//		_loadMode = LoadMode::Start;

	//		break;
	//		//�����Ȃ��E�E�E
	//	case LoadMode::Start:
	//		//���[�f�B���O�n��
	//		th = std::thread([&] {asyncLoad(); });
	//		_loadMode = LoadMode::Run;
	//		break;
	//	case LoadMode::Run:
	//		//���[�f�B���O���ɂ�肽������

	//		//������]
	//		for (int i = 0; i < 11; i++)
	//		{
	//			static float angle = 0.0f;
	//			angle += 0.5f;
	//			Now_Loading[i].get()->SetRot(angle);
	//			Now_Loading[i].get()->Update();
	//		}

	//		break;
	//	case LoadMode::End:
	//		//���[�f�B���O�I���
	//		th.join();
	//		load = false;

	//	default:
	//		break;
	//	}
	//}
}
