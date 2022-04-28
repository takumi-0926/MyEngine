#pragma once

enum {
	NONE,
	PARABOLA,
	AIR
};
struct Move {
	float gravity;		//重力
	float vy;			//Y方向
	float vx;			//X方向
	float v0;			//初速度
	float accel;		//加速度
	float air_resister;	//空気抵抗
	float time;			//時間
	float v;			//速度（計算結果）
	float k;			//比例定数

	bool flag;			//移動開始フラグ
	int moveNum;		//運動番号
};