#pragma once

enum {
	NONE,
	PARABOLA,
	AIR
};
struct Move {
	float gravity;		//�d��
	float vy;			//Y����
	float vx;			//X����
	float v0;			//�����x
	float accel;		//�����x
	float air_resister;	//��C��R
	float time;			//����
	float v;			//���x�i�v�Z���ʁj
	float k;			//���萔

	bool flag;			//�ړ��J�n�t���O
	int moveNum;		//�^���ԍ�
};