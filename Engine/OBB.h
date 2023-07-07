#pragma once
#include <DirectXMath.h>
#include "Math/Vector3.h"

class OBB {
protected:
	Vector3 pos;//位置
	Vector3 normalVector[3];//方向ベクトル
	float length[3];//ベクトルの長さ

public:
	OBB();
	static OBB* Create();

	Vector3 GetVector(int num) { return normalVector[num]; };//指定番号のベクトル
	float GetLength(int num) { return length[num]; };//指定番号のベクトルの長さ
	Vector3 GetPos() { return pos; };//位置

	void SetVector(int num, DirectX::XMVECTOR vec) { this->normalVector[num] = Vector3(vec.m128_f32[0], vec.m128_f32[1], vec.m128_f32[2]); }
	void SetLength(int num, float length) { this->length[num] = length;}
	void SetPos(DirectX::XMFLOAT3 pos) { this->pos = Vector3(pos.x, pos.y, pos.z); }
	void SetPos(DirectX::XMMATRIX pos) { this->pos = Vector3(pos.r[3].m128_f32[0], pos.r[3].m128_f32[1], pos.r[3].m128_f32[2]); }

};