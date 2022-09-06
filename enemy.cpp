#include "enemy.h"

float Enemy::objectDistance(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	float distance;
	float x = abs(pos1.x - pos2.x);
	float z = abs(pos1.z - pos2.z);
	distance = std::sqrt(x * 2 + z * 2);
	return distance;
}
XMVECTOR Enemy::objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	XMVECTOR distance;
	float x = pos1.x - pos2.x;
	float z = pos1.z - pos2.z;
	distance = { x,0,z,0 };
	return distance;

}
XMVECTOR Enemy::normalize(XMVECTOR vec)
{
	return XMVECTOR();
}
XMFLOAT3 Enemy::moveObject(XMFLOAT3 pos1, XMFLOAT3 pos2, float pct)
{
	XMFLOAT3 pos;
	pos.x = pos1.x + ((pos2.x - pos1.x) * pct);
	pos.z = pos1.z + ((pos2.z - pos1.z) * pct);
	pos.y = pos1.y;
	return pos;
}

Enemy::Enemy()
{
	status = {
			  1,//デフォルトHP
			  1,//デフォルト攻撃力
			  1,//デフォルト防御力
			  1,//デフォルト速度
		{1,1,1},//デフォルト大きさ
	};
	alive = false;
	step = 0.00005f;
	pct = 0.0f;
	mode = 0;
}
Enemy* Enemy::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	Enemy* object3d = new Enemy();
	if (object3d == nullptr) {
		return nullptr;
	}

	object3d->position.y = 20;
	object3d->position.z = -150;

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}
void Enemy::Update() {
	Object3Ds::Update();
}
void Enemy::Draw()
{
	Object3Ds::Draw();
}

void Enemy::moveUpdate(XMFLOAT3 pPos, DefCannon* bPos[], XMFLOAT3 gPos)
{
	static int d = 7;
	int objectNo = 0;

	this->oldPos = this->position;

	//移動処理
	if (this->move == true) {
		//パターン1
		if (this->mode == 1) {
			float distance = 1000;//距離保存用
			for (int i = 0; i < 6; i++)
			{
				//距離を測定して攻撃対象を決定
				float dis = objectDistance(this->position, bPos[i]->position);
				if (distance >= dis) {
					if (bPos[i]->isAlive != true) { continue; }
					distance = dis;
					objectNo = i;
				}
			}

			if (this->pct <= 1.0f) {
				this->position = moveObject(this->position, bPos[objectNo]->position, this->pct);
				this->pct += this->step;
			}

			//攻撃対象がなくなった場合
			for (int i = 0; i < 6; i++)
			{
				static int Num = 0;
				if (bPos[i]->isAlive == false) { Num++; }
				//行動パターンを変化
				if (Num == 6) { this->mode = 3; }
			}

			//移動から攻撃へ
			if (objectDistance(this->position, bPos[objectNo]->position) <= d) {
				this->attack = true;
				this->move = false;
				this->attackOnMove = false;
			}
		}
		//パターン2
		if (this->mode == 2) {
			if (this->pct <= 1.0f) {
				this->position = moveObject(this->position, pPos, this->pct);
				this->pct += this->step;
			}

			//移動から攻撃へ
			if (objectDistance(this->position, pPos) <= d) {
				this->attack = true;
				this->move = false;
			}
		}
		//パターン3
		if (this->mode == 3) {
			if (this->pct <= 0.1f) {
				this->position = moveObject(this->position, gPos, this->pct);
				this->pct += this->step;
			}

			//移動から攻撃へ
			if (objectDistance(this->position, gPos) <= d) {
				this->attack = true;
				this->move = false;
				this->attackOnMove = false;
			}
		}
	}

	//攻撃処理
	if (this->attack == true) {
		if (this->mode == 1) {
			//攻撃時の情報取得
			if (this->startAttack == false) {
				moveReset();
				this->vectol = objectVector(bPos[objectNo]->position, this->position);
				this->attackPos = this->position;
				this->startAttack = true;
			}

			if (this->attackTime >= 3.0f) {
				if (this->attackHit == true) {
					this->position.x += this->vectol.m128_f32[0] / 25;
					this->position.y += this->vectol.m128_f32[1] / 25;
					this->position.z += this->vectol.m128_f32[2] / 25;
					this->attackOnMove = true;
				}
				else {
					this->position = moveObject(this->position, this->attackPos, this->pct);
					this->pct += this->step;
				}
			}
			else if (this->attackTime >= 1.0f) {
				this->position.x -= this->vectol.m128_f32[0] / 100;
				this->position.y -= this->vectol.m128_f32[1] / 100;
				this->position.z -= this->vectol.m128_f32[2] / 100;
			}

			//移動処理移行時の初期化
			if (this->position.x == this->attackPos.x && this->attackOnMove == true) {
				if (this->position.y == this->attackPos.y) {
					if (this->position.z == this->attackPos.z) {
						moveReset();
						this->attackTime = 0.0f;
						this->move = true;
						this->attack = false;
						this->startAttack = false;
						this->attackHit = true;
					};
				};
			}

			this->attackTime += 1.0f / 60.0f;
		}
		if (this->mode == 2) {
			//攻撃時の情報取得
			if (this->startAttack == false) {
				this->vectol = objectVector(pPos, this->position);
				this->attackPos = this->position;
				this->startAttack = true;
			}

			//攻撃開始（突進）
			if (this->attackTime >= 3.0f) {
				this->position.x += vectol.m128_f32[0] / 25;
				this->position.y += vectol.m128_f32[1] / 25;
				this->position.z += vectol.m128_f32[2] / 25;
			}
			else if (this->attackTime >= 1.0f) {
				this->position.x -= this->vectol.m128_f32[0] / 100;
				this->position.y -= this->vectol.m128_f32[1] / 100;
				this->position.z -= this->vectol.m128_f32[2] / 100;
			}

			//移動処理移行時の初期化
			if (this->attackTime >= 5.0f) {
				moveReset();
				this->attackTime = 0.0f;
				this->move = true;
				this->attack = false;
				this->startAttack = false;
				this->attackHit = true;
			}

			this->attackTime += 1.0f / 60.0f;
		}
		if (this->mode == 3) {
			//攻撃時の情報取得
			if (this->startAttack == false) {
				moveReset();
				this->vectol = objectVector(gPos, this->position);
				this->attackPos = this->position;
				this->startAttack = true;
			}

			//攻撃開始（突進）
			if (this->attackTime >= 3.0f) {
				if (this->attackHit == true) {
					this->position.x += this->vectol.m128_f32[0] / 25;
					this->position.y += this->vectol.m128_f32[1] / 25;
					this->position.z += this->vectol.m128_f32[2] / 25;
					this->attackOnMove = true;
				}
				else {
					this->position = moveObject(this->position, this->attackPos, this->pct);
					this->pct += this->step;
				}
			}
			else if (this->attackTime >= 1.0f) {
				this->position.x -= this->vectol.m128_f32[0] / 100;
				this->position.y -= this->vectol.m128_f32[1] / 100;
				this->position.z -= this->vectol.m128_f32[2] / 100;
			}

			//移動処理移行時の初期化
			if (this->position.x == this->attackPos.x && this->attackOnMove == true) {
				if (this->position.y == this->attackPos.y) {
					if (this->position.z == this->attackPos.z) {
						moveReset();
						this->attackTime = 0.0f;
						this->move = true;
						this->attack = false;
						this->startAttack = false;
						this->attackHit = true;
					};
				};
			}

			this->attackTime += 1.0f / 60.0f;
		}
	}

	Object3Ds::Update();
	//Update();
}
void Enemy::moveReset()
{
	this->step = 0.00005f;	//進行
	this->pct = 0.0f;		//経過
}

