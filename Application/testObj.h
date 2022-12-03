#pragma once
#include "object\object3D.h"
#include "Math/Vector2.h"

#define	SX_SCREEN	1280
#define	SY_SCREEN	1024

#define	R_OBJ		 16		//���̂̔��a
#define	LENGTH_LINK	 1	//���̂̃����N�̒���
#define	K_HOOK		(0.01f)	//�t�b�N�̖@���̌W��K
#define	K_V_RES		(0.5f)	//�S����R�W��

#define	ACC_G		(-0.2f)	//�d�͉����x

#define	NUM_HIST	1000

#define	NUM_OBJ	10

#define PI 3.1415926535897932384626433832795
#define LENGTH      50                 // �R�̒���
#define CLENGTH     (LENGTH * 2 * PI)   // �R��L�΂��Ĉ���������ꍇ�ɏo����~�̉~���̒���
#define MASS        0.1346               // �Ԃ牺�����Ă��镨�̎���
#define G           -9.81                // �d�͉����x

class Obj : public Object3Ds
{
public:
	int	stat;	//0=���݂��Ȃ��A1=���݂��Ċ������A2=���݂��ĐÎ~
	int	graph;

	Vec2	pos;
	Vec2	size;
	Vec2	vel;
	Vec2	acc;
	double	m;
	double	r;
	double	dist;
	Vec2	hp[NUM_HIST];
	int		idx_hist;
	//	int	hit;
	int	idx;
	//	int hit_cnt;
	Obj* link0;	//�S���łȂ����Ă���OBJ
	Obj* link1;	//�S���łȂ����Ă���OBJ
	int	grabbed;	//�}�E�X�Œ͂܂�Ă���


	double x;     // �R��L�΂��Ĉ���������ꍇ�ɏo����~����̍��W�A�O�͕R�����̐^���ɂ���ʒu
	double speed; // x�̑��x
	double angle;
	int    jiku_x = 0, jiku_y = 0; // ���̈ʒu
	int nx, ny;

	Obj();

	static Obj *Create();
	bool Initialize() override;
	void Update() override;
	void Draw() override;
	void disp();
};
