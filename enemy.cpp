#include "enemy.h"

XMFLOAT3 Enemy::VectorToXMFloat(XMVECTOR vec)
{
	XMFLOAT3 ret;
	ret.x = vec.m128_f32[0];
	ret.y = vec.m128_f32[1];
	ret.z = vec.m128_f32[2];
	return ret;
}

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
XMVECTOR Enemy::Normalize(XMVECTOR vec)
{
	Vector3 ret;
	ret.x = vec.m128_f32[0];
	ret.y = vec.m128_f32[1];
	ret.z = vec.m128_f32[2];
	ret.normalize();

	XMVECTOR _ret;
	_ret.m128_f32[0] = ret.x;
	_ret.m128_f32[1] = ret.y;
	_ret.m128_f32[2] = ret.z;
	_ret.m128_f32[3] = 0;
	return _ret;
}
void Enemy::move(XMVECTOR vector)
{
	this->position.x -= vector.m128_f32[0] * this->status.speed;
	this->position.y -= vector.m128_f32[1] * this->status.speed;
	this->position.z -= vector.m128_f32[2] * this->status.speed;
}

Enemy::Enemy()
{
	status = {
			  2,//�f�t�H���gHP
			  1,//�f�t�H���g�U����
			  1,//�f�t�H���g�h���
			  0.2,//�f�t�H���g���x
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
	if (this->Move == true) {
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

			move(Normalize(objectVector(this->position, bPos[objectNo]->position)));
			this->matRot = LookAtRotation(
				VectorToXMFloat(Normalize(objectVector(this->position, bPos[objectNo]->position))),
				XMFLOAT3(0.0f, 1.0f, 0.0f));

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
				this->Move = false;
				this->attackOnMove = false;
			}
		}
		//�p�^�[��2
		if (this->mode == 2) {
			move(Normalize(objectVector(this->position, pPos)));
			this->matRot = LookAtRotation(
				VectorToXMFloat(Normalize(objectVector(this->position, pPos))),
				XMFLOAT3(0.0f, 1.0f, 0.0f));

			//�ړ�����U����
			if (objectDistance(this->position, pPos) <= d) {
				this->attack = true;
				this->Move = false;
			}
		}
		//�p�^�[��3
		if (this->mode == 3) {
			move(Normalize(objectVector(this->position, gPos)));
			this->matRot = LookAtRotation(
				VectorToXMFloat(Normalize(objectVector(this->position, gPos))),
				XMFLOAT3(0.0f, 1.0f, 0.0f));

			//�ړ�����U����
			if (objectDistance(this->position, gPos) <= d) {
				this->attack = true;
				this->Move = false;
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
					move(Normalize(objectVector(this->position, bPos[objectNo]->position)));
					this->matRot = LookAtRotation(
						VectorToXMFloat(Normalize(objectVector(this->position, gPos))),
						XMFLOAT3(0.0f, 1.0f, 0.0f));

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
						this->Move = true;
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
				this->Move = true;
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
					move(Normalize(objectVector(this->position, bPos[objectNo]->position)));
					this->matRot = LookAtRotation(
						VectorToXMFloat(Normalize(objectVector(this->position, bPos[objectNo]->position))),
						XMFLOAT3(0.0f, 1.0f, 0.0f));
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
						this->Move = true;
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

