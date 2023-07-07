#pragma once
#include <DirectXMath.h>
#include "Math/Matrix4.h"
#include <Math/Quaternion.h>

using namespace DirectX;

extern const float fps;

inline XMMATRIX ExtractScaleMat(XMMATRIX matworld)
{
	return XMMatrixScaling(
		XMVector3Length(XMVECTOR{
			matworld.r[0].m128_f32[0], matworld.r[0].m128_f32[1], matworld.r[0].m128_f32[2]
			}).m128_f32[0],
		XMVector3Length(XMVECTOR{
			matworld.r[1].m128_f32[0], matworld.r[1].m128_f32[1], matworld.r[1].m128_f32[2]
			}).m128_f32[0],
		XMVector3Length(XMVECTOR{
			matworld.r[2].m128_f32[0], matworld.r[2].m128_f32[1], matworld.r[2].m128_f32[2]
			}).m128_f32[0]
	);
}

inline XMMATRIX ExtractPositionMat(XMMATRIX matworld)
{
	return XMMatrixTranslation(matworld.r[3].m128_f32[0], matworld.r[3].m128_f32[1], matworld.r[3].m128_f32[2]);
}

inline XMMATRIX ExtractRotationMat(XMMATRIX matworld)
{
	XMMATRIX mOffset = ExtractPositionMat(matworld);
	XMMATRIX mScaling = ExtractScaleMat(matworld);

	XMVECTOR det;
	// 左からScaling、右からOffsetの逆行列をそれぞれかける。
	return XMMatrixInverse(&det, mScaling) * matworld * XMMatrixInverse(&det, mOffset);
}

inline XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward)
{
	Vector3 z = Vector3(forward.x, forward.y, forward.z);//進行方向ベクトル（前方向）
	Vector3 up = Vector3(upward.x, upward.y, upward.z);  //上方向
	XMMATRIX rot;//回転行列
	Quaternion q = quaternion(0, 0, 0, 1);//回転クォータニオン
	Vector3 _z = { 0.0f,0.0f,1.0f };//Z方向単位ベクトル
	Vector3 cross;
	XMMATRIX matRot = XMMatrixIdentity();

	float a;//角度保存用
	float b;//角度保存用
	float c;//角度保存用
	float d;//角度保存用

	cross = z.cross(_z);

	q.x = cross.x;
	q.y = cross.y;
	q.z = cross.z;

	q.w = float(sqrt((z.length() * z.length()) * (_z.length() * _z.length())) + z.dot(_z));

	//単位クォータニオン化
	q = normalize(q);
	q = conjugate(q);
	a = q.x;
	b = q.y;
	c = q.z;
	d = q.w;

	//任意軸回転
	XMVECTOR rq = { q.x,q.y,q.z,q.w };
	rot = XMMatrixRotationQuaternion(rq);

	return rot;
}

using float3 = XMFLOAT3;

inline const float3& operator-=(float3& f1, const float3& f2) {
	f1.x = f1.x - f2.x;
	f1.y = f1.y - f2.y;
	f1.z = f1.z - f2.z;

	return f1;
}

inline const float3 operator-(const float3& v1, const float3& v2) {
	float3 temp(v1);
	return temp -= v2;
}

inline const float3& operator+=(float3& f1, const float3& f2) {
	f1.x = f1.x + f2.x;
	f1.y = f1.y + f2.y;
	f1.z = f1.z + f2.z;
	return f1;
}

inline const float3 operator+(const float3& v1, const float3& v2) {
	float3 temp(v1);
	return temp += v2;
}

inline const float3 operator*(const float3& v1, const float& f) {
	float3 temp(v1);
	temp.x *= f;
	temp.y *= f;
	temp.z *= f;
	return temp;
}
