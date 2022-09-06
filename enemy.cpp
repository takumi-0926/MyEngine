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
			  1,//�f�t�H���gHP
			  1,//�f�t�H���g�U����
			  1,//�f�t�H���g�h���
			  1,//�f�t�H���g���x
		{1,1,1},//�f�t�H���g�傫��
	};
	alive = false;
	step = 0.00005f;
	pct = 0.0f;
	mode = 0;
}
Enemy* Enemy::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Enemy* object3d = new Enemy();
	if (object3d == nullptr) {
		return nullptr;
	}

	object3d->position.y = 20;
	object3d->position.z = -150;

	// ������
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

	//�ړ�����
	if (this->move == true) {
		//�p�^�[��1
		if (this->mode == 1) {
			float distance = 1000;//�����ۑ��p
			for (int i = 0; i < 6; i++)
			{
				//�����𑪒肵�čU���Ώۂ�����
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

			//�U���Ώۂ��Ȃ��Ȃ����ꍇ
			for (int i = 0; i < 6; i++)
			{
				static int Num = 0;
				if (bPos[i]->isAlive == false) { Num++; }
				//�s���p�^�[����ω�
				if (Num == 6) { this->mode = 3; }
			}

			//�ړ�����U����
			if (objectDistance(this->position, bPos[objectNo]->position) <= d) {
				this->attack = true;
				this->move = false;
				this->attackOnMove = false;
			}
		}
		//�p�^�[��2
		if (this->mode == 2) {
			if (this->pct <= 1.0f) {
				this->position = moveObject(this->position, pPos, this->pct);
				this->pct += this->step;
			}

			//�ړ�����U����
			if (objectDistance(this->position, pPos) <= d) {
				this->attack = true;
				this->move = false;
			}
		}
		//�p�^�[��3
		if (this->mode == 3) {
			if (this->pct <= 0.1f) {
				this->position = moveObject(this->position, gPos, this->pct);
				this->pct += this->step;
			}

			//�ړ�����U����
			if (objectDistance(this->position, gPos) <= d) {
				this->attack = true;
				this->move = false;
				this->attackOnMove = false;
			}
		}
	}

	//�U������
	if (this->attack == true) {
		if (this->mode == 1) {
			//�U�����̏��擾
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

			//�ړ������ڍs���̏�����
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
			//�U�����̏��擾
			if (this->startAttack == false) {
				this->vectol = objectVector(pPos, this->position);
				this->attackPos = this->position;
				this->startAttack = true;
			}

			//�U���J�n�i�ːi�j
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

			//�ړ������ڍs���̏�����
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
			//�U�����̏��擾
			if (this->startAttack == false) {
				moveReset();
				this->vectol = objectVector(gPos, this->position);
				this->attackPos = this->position;
				this->startAttack = true;
			}

			//�U���J�n�i�ːi�j
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

			//�ړ������ڍs���̏�����
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
	this->step = 0.00005f;	//�i�s
	this->pct = 0.0f;		//�o��
}

