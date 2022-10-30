#include "Player.h"

Player* Player::Create(PMDmodel* _model)
{
	//�C���X�^���X����
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	//������
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
