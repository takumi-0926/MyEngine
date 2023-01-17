#include "DefCannon.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

DefCannon::DefCannon()
{
}
DefCannon* DefCannon::Create(int _mode, Model* _model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	DefCannon* instance = new DefCannon();
	if (instance == nullptr) {
		return nullptr;
	}

	instance->position.y = 10;
	instance->shotVec = { 0,0,1,0 };
	// ������
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	if (_model) {
		instance->SetModel(_model);
	}

	instance->SetMode(_mode);
	return instance;
}
void DefCannon::BulletCreate(Model* _model)
{
	for (int i = 0; i < 10; i++)
	{
		Bullet* _bullet;
		_bullet = _bullet->Create(_model);
		_bullet->scale = { 1,1,1 };
		_bullet->SetAlive(false);
		_bullet->SetSpeed(4.0f);
		bullet.push_back(_bullet);
	}
}
void DefCannon::Update() {

	count += 1.0f / 60.0f;

	for (int i = 0; i < bullet.size(); i++) {
		if (bullet[i]->GetStatus().isAlive == false) {
			bullet[i]->SetBasePos(XMFLOAT3(position.x, position.y + 2.0f, position.z));
		}

		if (count >= bullet[i]->attackCount) {
			//��ԏ���(�g���Ă��Ȃ���ΗD��I�Ɏg��)
			if (i == 0 && bullet[i]->GetStatus().isAlive == false) {
				bullet[i]->SetAlive(true);
				bullet[i]->Update();
				count = 0;//�J�E���g���[����
				continue;
			}
			if (i != 0) {
				//�ЂƂO�̒e��������Ă��Ȃ��Ƃ�
				if (bullet[i - 1]->GetStatus().isAlive == false) {
					bullet[i]->Update();
					continue;
				}
				//�e�ȑO�̒e���S�Đ����Ă��Ď����������Ă��Ȃ�
				else if (bullet[i]->GetStatus().isAlive == false) {
					bullet[i]->SetAlive(true);
					count = 0;//�J�E���g���[����
				}
			}
		}
		bullet[i]->Update();
	}

	Object3Ds::Update();
}
void DefCannon::Draw()
{
	Object3Ds::Draw();

	for (int i = 0; i < bullet.size(); i++) {
		if (!bullet[i]->GetStatus().isAlive) { continue; }
		bullet[i]->Draw();
	}
}

float DefCannon::distance(XMFLOAT3 pos1, XMFLOAT3 pos2)
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
XMVECTOR DefCannon::Normalize(XMVECTOR vec)
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
XMFLOAT3 DefCannon::VectorToXMFloat(XMVECTOR vec)
{
	XMFLOAT3 ret;
	ret.x = vec.m128_f32[0];
	ret.y = vec.m128_f32[1];
	ret.z = vec.m128_f32[2];
	return ret;
}

void DefCannon::moveUpdate(std::vector<Enemy*> ePos)
{
	AutoShot(ePos);

	FixedShot();

	//matRot = LookAtRotation(VectorToXMFloat(Normalize(objectVector(ePos[No]->GetPosition(), position))), XMFLOAT3(0, 1, 0));
}

DefCannon* DefCannon::Appearance(int type, Model* hand, Model* assault, Model* sniper)
{
	DefCannon* obj = nullptr;
	if (type == DefenceType::Hand) { obj = DefCannon::Create(ShotMode::Fixed, hand); }
	else if (type == DefenceType::Assault) { obj = DefCannon::Create(ShotMode::Fixed, assault); }
	else if (type == DefenceType::Sniper) { obj = DefCannon::Create(ShotMode::Fixed, sniper); }

	return obj;
}

void DefCannon::AutoShot(std::vector<Enemy*> ePos)
{
	if (mode != ShotMode::Auto) { return; }

	int No = 0;
	//�G�l�~�[�����Ȃ��Ȃ甲����
	if (ePos.size() == 0) { return; }

	for (int i = 0; i < bullet.size(); i++)
	{
		//�G�l�~�[�����Ȃ��Ȃ甲����
		if (ePos.size() == 0) { break; }

		float _distance = 1000;
		for (int j = 0; j < ePos.size(); j++)
		{
			float dis = distance(position, ePos[j]->GetPosition());
			if (dis <= _distance) {
				_distance = dis;
				No = j;
			}
		}

		//�����Ă��Ȃ�
		if (bullet[i]->GetStatus().isAlive == false) { continue; }
		//�ڕW����܂��Ă���
		if (bullet[i]->GetStatus().vecSet != false) { continue; }
		bullet[i]->SetVec(objectVector(ePos[No]->GetPosition(), bullet[i]->GetStatus().basePos));
		bullet[i]->SetVec(XMVector3Normalize(bullet[i]->GetStatus().vec));
		bullet[i]->SetVecSet(true);
	}
}

void DefCannon::FixedShot()
{
	if (mode != ShotMode::Fixed) { return; }

	for (int i = 0; i < bullet.size(); i++)
	{
		//�����Ă��Ȃ�
		if (bullet[i]->GetStatus().isAlive != false) { continue; }
		////�ڕW����܂��Ă���
		//if (bullet[i]->GetStatus().vecSet != false) { continue; }
		bullet[i]->SetVec(shotVec);
		bullet[i]->SetVec(XMVector3Normalize(bullet[i]->GetStatus().vec));
		bullet[i]->SetVecSet(true);
		bullet[i]->Update();
	}
}

XMMATRIX DefCannon::LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward)
{
	Vector3 z = Vector3(forward.x, forward.y, forward.z);//�i�s�����x�N�g���i�O�����j
	Vector3 up = Vector3(upward.x, upward.y, upward.z);  //�����
	XMMATRIX rot;//��]�s��
	Quaternion q = quaternion(0, 0, 0, 1);//��]�N�H�[�^�j�I��
	Vector3 _z = { 0.0f,0.0f,-1.0f };//Z�����P�ʃx�N�g��
	Vector3 cross;

	float a;//�p�x�ۑ��p
	float b;//�p�x�ۑ��p
	float c;//�p�x�ۑ��p
	float d;//�p�x�ۑ��p

	cross = z.cross(_z);

	q.x = cross.x;
	q.y = cross.y;
	q.z = cross.z;

	q.w = sqrt(
		(z.length() * z.length())
		* (_z.length() * _z.length())) + z.dot(_z);

	//�P�ʃN�H�[�^�j�I����

	q = normalize(q);
	q = conjugate(q);
	a = q.x;
	b = q.y;
	c = q.z;
	d = q.w;

	//�C�ӎ���]
	XMVECTOR rq = { q.x,q.y,q.z,q.w };
	rot = XMMatrixRotationQuaternion(rq);

	this->useRotMat = true;
	return rot;
}
