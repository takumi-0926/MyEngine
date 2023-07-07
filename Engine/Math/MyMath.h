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
	// ������Scaling�A�E����Offset�̋t�s������ꂼ�ꂩ����B
	return XMMatrixInverse(&det, mScaling) * matworld * XMMatrixInverse(&det, mOffset);
}

inline XMMATRIX LookAtRotation(XMFLOAT3 forward, XMFLOAT3 upward)
{
	Vector3 z = Vector3(forward.x, forward.y, forward.z);//�i�s�����x�N�g���i�O�����j
	Vector3 up = Vector3(upward.x, upward.y, upward.z);  //�����
	XMMATRIX rot;//��]�s��
	Quaternion q = quaternion(0, 0, 0, 1);//��]�N�H�[�^�j�I��
	Vector3 _z = { 0.0f,0.0f,1.0f };//Z�����P�ʃx�N�g��
	Vector3 cross;
	XMMATRIX matRot = XMMatrixIdentity();

	float a;//�p�x�ۑ��p
	float b;//�p�x�ۑ��p
	float c;//�p�x�ۑ��p
	float d;//�p�x�ۑ��p

	cross = z.cross(_z);

	q.x = cross.x;
	q.y = cross.y;
	q.z = cross.z;

	q.w = float(sqrt((z.length() * z.length()) * (_z.length() * _z.length())) + z.dot(_z));

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