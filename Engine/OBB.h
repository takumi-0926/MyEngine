#pragma once
#include <DirectXMath.h>
#include "Math/Vector3.h"

class OBB {
protected:
	Vector3 pos;//�ʒu
	Vector3 normalVector[3];//�����x�N�g��
	float length[3];//�x�N�g���̒���

public:
	OBB();
	static OBB* Create();

	Vector3 GetVector(int num) { return normalVector[num]; };//�w��ԍ��̃x�N�g��
	float GetLength(int num) { return length[num]; };//�w��ԍ��̃x�N�g���̒���
	Vector3 GetPos() { return pos; };//�ʒu

	void SetVector(int num, DirectX::XMVECTOR vec) { this->normalVector[num] = Vector3(vec.m128_f32[0], vec.m128_f32[1], vec.m128_f32[2]); }
	void SetLength(int num, float length) { this->length[num] = length;}
	void SetPos(DirectX::XMFLOAT3 pos) { this->pos = Vector3(pos.x, pos.y, pos.z); }
	void SetPos(DirectX::XMMATRIX pos) { this->pos = Vector3(pos.r[3].m128_f32[0], pos.r[3].m128_f32[1], pos.r[3].m128_f32[2]); }

};