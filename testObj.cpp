#include "testObj.h"

Obj::Obj()
{
	pos.clear(0);
	r = R_OBJ;
	dist = 0;
	vel.clear(0);
	acc.clear(0);
	m = 1.0f;
	stat = 0;
	idx_hist = 0;
	//	hit = 0;
	//	hit_cnt = 0;
	for (int i = 0; i < NUM_HIST; i++)
	{
		hp[i].clear(0);
	}
	link0 = NULL;
	link1 = NULL;
	grabbed = 0;

	// �����ʒu�͎��̐^�����獶������45�x�X�����ʒu
	x = CLENGTH / 8.0;

	// �����x�͂O
	speed = 0.0;
}

Obj* Obj::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Obj* object3d = new Obj();
	if (object3d == nullptr) {
		return nullptr;
	}

	// ������
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}

bool Obj::Initialize()
{
	Object3Ds::Initialize();
	return true;
}

void Obj::Update()
{
	Object3Ds::Update();


	//hp[idx_hist % NUM_HIST].x = pos.x;
	//hp[idx_hist % NUM_HIST].y = pos.y;
	//idx_hist++;

	//pos.x = position.x;
	//pos.y = position.y;

	////if (grabbed)
	////{
	////	pos = MousePos;
	////	if (!MouseInputLeft)
	////		grabbed = 0;
	////}
	////else
	////{
	////	double _dst = pos.distance(MousePos);
	////	if (MouseInputLeft)
	////	{
	////		if (_dst < r)
	////			grabbed = 1;
	////	}

	////}

	//// 0:���݂��ĂȂ��@1:���݂��Ċ������@2:���݂��ĂĊ������ĂȂ�
	//if (stat == 2)
	//	return;

	//{
	//	// acc:�����x
	//	acc.x = 0.0f;
	//	acc.y = ACC_G;	//�d�͉����x�i�l�͓K���ŗǂ��j

	//	if (link0)	//�eOBJ������?
	//	{
	//		// ������1���obj�ւ̃x�N�g��
	//		Vec2	_dist = link0->pos - pos;
	//		// ���̃x�N�g���̑傫��
	//		double	_len = _dist.length();

	//		// �eobj�Ƃ̋������W�������iLENGTH_LINK�j���傫�����
	//		// �����x�����Z
	//		if (_len > LENGTH_LINK)
	//		{
	//			// �x�N�g��(_dist)���W����������̑���
	//			_dist = _dist * (_len - LENGTH_LINK);

	//			_dist /= LENGTH_LINK;

	//			// K_HOOK:�΂˒萔�i�L�тɂ����j
	//			// ����(m)���傫���ق�_dist���|���������x(acc)�̑����͌���
	//			Vec2 add_acc = _dist * K_HOOK / m;
	//			acc += add_acc;
	//		}
	//	}
	//	if (link1)	//�qOBJ������?
	//	{
	//		// �����̂P����obj�ւ̃x�N�g��
	//		Vec2	_dist = link1->pos - pos;
	//		// ���̃x�N�g���̑傫��
	//		double	_len = _dist.length();

	//		if (_len > LENGTH_LINK)
	//		{
	//			_dist = _dist * (_len - LENGTH_LINK) / LENGTH_LINK;
	//			acc += _dist * K_HOOK / m;
	//		}
	//	}

	//	vel += acc;			//���x+=�����x

	//	vel -= vel * K_V_RES;	//�S����R
	//							// (���̂��ߖT�̗��̂��������邱�Ƃɂ���Ď󂯂锽��p)

	//	pos += vel;			//�ʒu+=���x

	//	position.x = pos.x;
	//	position.y = pos.y;
	//}



		// �����ɏ]���đ��x�����Z
		// MASS�̒l������������Ƃ�����蓮��
		speed += -MASS * G * sin(x / LENGTH);

		// ���x�ɏ]���ĉ~��̍��W��ύX
		x += speed;

		// �������_�Ƃ����ꍇ�̂Ԃ牺�����Ă��镨�̍��W���Z�o
		// ���̂܂܂��Ɓ|45�`45�x�̐U��q�ɂȂ�̂�
		// ���v����90�x�iPI/2.0�j��]
		angle = x / LENGTH + PI / 2.0;

		// ���߂��p�x���玲�����_�Ƃ����~����̍��W���擾
		nx = cos(angle) * LENGTH;
		ny = sin(angle) * LENGTH;

		position.x = jiku_x + nx;
		position.y = jiku_y + ny;
}

void Obj::Draw()
{
	Object3Ds::Draw();
}

void Obj::disp()
{
	double	x = pos.x + SX_SCREEN / 2;
	double	y = pos.y + SY_SCREEN / 2;

	//DrawCircle(x, y, r, GetColor(128, 128, 128), 1, 1);

	//link�̕\��
	if (link0)
	{
		double	xl = link0->pos.x + SX_SCREEN / 2;
		double	yl = link0->pos.y + SY_SCREEN / 2;
		//DrawLine(x, y, xl, yl, GetColor(255, 255, 255));//����`��
	}
}
