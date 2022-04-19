#pragma once

struct Move {
	float gravity;		//重力
	float vy;			//Y方向
	float vx;			//X方向
	float v0;			//初速度
	float accel;		//加速度
	float air_resister;	//空気抵抗
	float time;			//時間
	float v;			//速度（計算結果）

	bool flag;			//移動開始フラグ
};