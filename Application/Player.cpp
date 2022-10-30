#include "Player.h"

Player* Player::Create(PMDmodel* _model)
{
	//インスタンス生成
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(_model)) {
		delete instance;
		assert(0);
	}

	return instance;
}

void Player::Update()
{
	PMDobject::Update();
}

void Player::Draw()
{
	PMDobject::Draw();
}
