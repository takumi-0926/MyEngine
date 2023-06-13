#include "EnemyManager.h"

EnemyManager::EnemyManager()
{
}

EnemyManager::~EnemyManager()
{
}

EnemyManager* EnemyManager::Create(FbxModel* model1, FbxModel* model2, FbxModel* model3)
{
	if (model1 == nullptr) { assert(1); }

	//�C���X�^���X����
	EnemyManager* instance = new EnemyManager();

	instance->SetModel_1(model1);

	if (model2 != nullptr) {
		instance->SetModel_2(model2);
	}

	return instance;
}

void EnemyManager::EnemyCreate()
{
	//golem.push_back(Enemy::Create(GolemModel,Activity::golem));
	wolf.push_back(Enemy::Create(WolfModel, Activity::wolf));

	enemyNum += 1;
}

void EnemyManager::Update(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{
	//�������ԂɂȂ�A�����Ώۂ������Ă��Ȃ��Ȃ�
	enemyPopTime += 1.0f / 60.0f;
	if (enemyPopTime >= 5.0f) {

		EnemyCreate();

		//�g��
		//for (int i = 0; i < 3; i++) {
		//	if (protEnemy[i]->alive) { continue; }

		//	protEnemy[i]->Appearance();
		//	_enemy.push_back(protEnemy[i]);

		//	//�����蔻��p���̐���
		//	Sqhere _sqhere;
		//	_sqhere.radius = 20.0f;
		//	sqhere.push_back(_sqhere);

		enemyPopTime = 0.0f;

		//	break;
		//}
	}
	//�G�l�~�[�֌W�̐���
	{
		for (auto& _enemy : golem) {
			_enemy->moveUpdate(pPos,bPos,gPos);
			_enemy->Update();
		}
		for (auto& _enemy : wolf) {
			_enemy->moveUpdate(pPos, bPos, gPos);
			_enemy->Update();
		}

		//�g��
		//for (int i = 0; i < _enemy.size(); i++) {
		//	_enemy[i]->moveUpdate(_player->GetPosition(), defense_facilities, stages[64]->position);
		//	sqhere[i].center = XMVectorSet(_enemy[i]->GetPosition().x, _enemy[i]->GetPosition().y + 10.0f, _enemy[i]->GetPosition().z, 1);
		//	if (_enemy[i]->alive) { continue; }
		//	_enemy.erase(_enemy.begin() + i);
		//	sqhere.erase(sqhere.begin() + i);
		//}
	}

}

void EnemyManager::Draw(ID3D12GraphicsCommandList* cmdList, bool shadow)
{
	if (shadow) {
		for (auto& _enemy : golem) {
			_enemy->ShadowDraw(cmdList);
		}
		for (auto& _enemy : wolf) {
			_enemy->ShadowDraw(cmdList);
		}
	}
	else {
		for (auto& _enemy : golem) {
			_enemy->Draw(cmdList);
		}
		for (auto& _enemy : wolf) {
			_enemy->Draw(cmdList);
		}
	}
}
