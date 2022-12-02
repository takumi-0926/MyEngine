#pragma once
#include "object\object3D.h"
#include "Math/Vector2.h"

#define	SX_SCREEN	1280
#define	SY_SCREEN	1024

#define	R_OBJ		 16		//物体の半径
#define	LENGTH_LINK	 1	//物体のリンクの長さ
#define	K_HOOK		(0.01f)	//フックの法則の係数K
#define	K_V_RES		(0.5f)	//粘性抵抗係数

#define	ACC_G		(-0.2f)	//重力加速度

#define	NUM_HIST	1000

#define	NUM_OBJ	10

#define PI 3.1415926535897932384626433832795
#define LENGTH      50                 // 紐の長さ
#define CLENGTH     (LENGTH * 2 * PI)   // 紐を伸ばして一周させた場合に出来る円の円周の長さ
#define MASS        0.1346               // ぶら下がっている物の質量
#define G           -9.81                // 重力加速度

class Obj : public Object3Ds
{
public:
	int	stat;	//0=存在しない、1=存在して活動中、2=存在して静止
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
	Obj* link0;	//ゴムでつながっているOBJ
	Obj* link1;	//ゴムでつながっているOBJ
	int	grabbed;	//マウスで掴まれている


	double x;     // 紐を伸ばして一周させた場合に出来る円周上の座標、０は紐が軸の真下にいる位置
	double speed; // xの速度
	double angle;
	int    jiku_x = 0, jiku_y = 0; // 軸の位置
	int nx, ny;

	Obj();

	static Obj *Create();
	bool Initialize() override;
	void Update() override;
	void Draw() override;
	void disp();
};
