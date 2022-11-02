#include "Player.h"
#include "Input/input.h"

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
	if (model->position.x <= -100.0f) {
		model->position.x = -100.0f;
	}
	if (model->position.x >= 100.0f) {
		model->position.x = 100.0f;
	}
	if (model->position.z <= -100.0f) {
		model->position.z = -100.0f;
	}
	if (model->position.z >= 322.0f) {
		model->position.z = 322.0f;
	}

	//移動
	PMDobject::Update();
}

void Player::Draw()
{
	PMDobject::Draw();
}
