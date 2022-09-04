#include "DefCannon.h"

DefCannon::DefCannon()
{
}
DefCannon* DefCannon::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	DefCannon* object3d = new DefCannon();
	if (object3d == nullptr) {
		return nullptr;
	}

	object3d->position.y = 0;

	// ������
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}
void DefCannon::BulletInit()
{
	for (int i = 0; i < 10; i++)
	{
		Bullet* _bullet;
		_bullet = _bullet->Create();
		_bullet->SetModel(model);
		_bullet->scale = { 1,1,1 };
		_bullet->status.isAlive = false;
		_bullet->status.speed = 1.0f;
		bullet.push_back(_bullet);
	}
}
void DefCannon::Update() {
	Object3Ds::Update();

	count += 1.0f / 60.0f;

	for (int i = 0; i < bullet.size(); i++) {
		if (bullet[i]->status.isAlive == false) {
			bullet[i]->status.basePos = position;
		}

		if (count >= bullet[i]->attackCount) {
			//��ԏ���(�g���Ă��Ȃ���ΗD��I�Ɏg��)
			if (i == 0 && bullet[i]->status.isAlive == false) {
				bullet[i]->status.isAlive = true;
				bullet[i]->Update();
				count = 0;//�J�E���g���[����
				continue;
			}
			if (i != 0) {
				//�ЂƂO�̒e��������Ă��Ȃ��Ƃ�
				if (bullet[i - 1]->status.isAlive == false) {
					bullet[i]->Update();
					continue;
				}
				//�e�ȑO�̒e���S�Đ����Ă��Ď����������Ă��Ȃ�
				else if(bullet[i]->status.isAlive == false) {
					bullet[i]->status.isAlive = true;
					count = 0;//�J�E���g���[����
				}
			}
		}

		bullet[i]->Update();
	}
}
void DefCannon::Draw()
{
	Object3Ds::Draw();

	for (int i = 0; i < bullet.size(); i++) {
		bullet[i]->Draw();
	}
}

float	 DefCannon::distance(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	float distance;
	float x = abs(pos1.x - pos2.x);
	float z = abs(pos1.z - pos2.z);
	distance = std::sqrt(x * 2 + z * 2);
	return distance;
}
XMVECTOR DefCannon::objectVector(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	XMVECTOR distance;
	float x = pos1.x - pos2.x;
	float y = pos1.y - pos2.y;
	float z = pos1.z - pos2.z;
	distance = { x,y,z,0 };
	return distance;
}
XMVECTOR DefCannon::normalize(XMVECTOR vec)
{
	float t; //�x�N�g���̑傫��
	float x = vec.m128_f32[0] * vec.m128_f32[0];//x����
	float y = vec.m128_f32[1] * vec.m128_f32[1];//y����
	float z = vec.m128_f32[2] * vec.m128_f32[2];//z����
	//�x�N�g���̑傫�����v�Z
	t = sqrt(x * x + y * y + z * z);
	//���K��
	XMVECTOR _vec;
	_vec.m128_f32[0] = vec.m128_f32[0] / t;
	_vec.m128_f32[1] = vec.m128_f32[1] / t;
	_vec.m128_f32[2] = vec.m128_f32[2] / t;

	return _vec;
}

void DefCannon::moveUpdate(std::vector<Enemy*> ePos)
{
	int No = 0;
	for (int i = 0; i < bullet.size(); i++)
	{
		//�G�l�~�[�����Ȃ��Ȃ甲����
		if (ePos.size() == 0) { break; }

		float _distance = 1000;
		for (int j = 0; j < ePos.size(); j++)
		{
			float dis = distance(position, ePos[j]->position);
			if (dis <= _distance) {
				_distance = dis;
				No = j;
			}
		}

		//�����Ă��Ȃ�
		if (bullet[i]->status.isAlive == false) { continue; }
		//�ڕW����܂��Ă���
		if (bullet[i]->status.vecSet != false) { continue; }
		bullet[i]->status.vec = objectVector(ePos[No]->position, position);
		bullet[i]->status.vec = XMVector3Normalize(bullet[i]->status.vec);
		bullet[i]->status.vecSet = true;
	}
}
