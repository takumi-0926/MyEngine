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

	//インスタンス生成
	EnemyManager* instance = new EnemyManager();

	instance->SetModel_1(model1);

	if (model2 != nullptr) {
		instance->SetModel_2(model2);
	}

	return instance;
}

void EnemyManager::EnemyCreate()
{
	if (enemyNum >= 1)return;
	//golem.push_back(Enemy::Create(GolemModel,Activity::golem));
	wolf.push_back(Enemy::Create(WolfModel, Activity::wolf));

	enemyNum += 1;
}

void EnemyManager::Update(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{
	//生成時間になり、生成対象が生きていないなら
	enemyPopTime += 1.0f / 60.0f;
	if (enemyPopTime >= 5.0f) {

		EnemyCreate();

		enemyPopTime = 0.0f;
	}
	//エネミー関係の制御
	{
		for (auto& _enemy : golem) {
			if (!_enemy->GetAlive()) {
				//num++;
				continue;
			}

			_enemy->moveUpdate(pPos, bPos, gPos);
			_enemy->Update();

		}
		for (auto& _enemy : wolf) {
			if (!_enemy->GetAlive()) {
				//num++;
				continue;
			}

			_enemy->moveUpdate(pPos, bPos, gPos);
			_enemy->Update();

		}

		//golem.remove_if([](std::unique_ptr<Enemy>& enemy) {return !enemy->GetAlive();  });
		//wolf.remove_if([](std::unique_ptr<Enemy>& enemy) {return !enemy->GetAlive();  });

		//使う
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
