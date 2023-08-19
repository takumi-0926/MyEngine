#pragma once
#include <DirectXMath.h>
#include "Math/Matrix4.h"
#include <Math/Quaternion.h>

using namespace DirectX;

extern const float fps;

//XMFLOAT3�^�֌W���Z�q�I�[�o�[���[�h
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

//�X�P�[�����o
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
//�ʒu���o
inline XMMATRIX ExtractPositionMat(XMMATRIX matworld)
{
	return XMMatrixTranslation(matworld.r[3].m128_f32[0], matworld.r[3].m128_f32[1], matworld.r[3].m128_f32[2]);
}
//��]���o
inline XMMATRIX ExtractRotationMat(XMMATRIX matworld)
{
	XMMATRIX mOffset = ExtractPositionMat(matworld);
	XMMATRIX mScaling = ExtractScaleMat(matworld);

	XMVECTOR det;
	// ������Scaling�A�E����Offset�̋t�s������ꂼ�ꂩ����B
	return XMMatrixInverse(&det, mScaling) * matworld * XMMatrixInverse(&det, mOffset);
}
//��]
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

//�ړ�
inline XMFLOAT3 moveCamera(const XMFLOAT3 pos1, const XMFLOAT3 pos2, float pct)
{
	XMFLOAT3 pos{};
	pos = pos1 + ((pos2 - pos1) * pct);
	return pos;
}


inline XMVECTOR moveVectortoVector(const XMVECTOR pos1, const XMVECTOR pos2, float pct)
{
	XMFLOAT3 answer{};
	XMFLOAT3 pos1_ = XMFLOAT3(pos1.m128_f32);
	XMFLOAT3 pos2_ = XMFLOAT3(pos2.m128_f32);

	answer = pos1_ + ((pos2_ - pos1_) * pct);

	return XMVECTOR{ answer.x,answer.y,answer.z,0 };
}

//����
inline float distance(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	float distance{};
	float x = abs(pos1.x - pos2.x);
	float z = abs(pos1.z - pos2.z);
	distance = float(sqrt(x * 2 + z * 2));
	return distance;
}

//Vector�^��XMFLAT3�^�ɃL���X�g
inline XMFLOAT3 VectorToXMFloat(XMVECTOR vec)
{
	XMFLOAT3 ret{};
	ret.x = vec.m128_f32[0];
	ret.y = vec.m128_f32[1];
	ret.z = vec.m128_f32[2];
	return ret;
}

//��_�Ԃ̃x�N�g�����Z�o
inline XMVECTOR twoPointVector(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	XMVECTOR distance{};
	float x = pos1.x - pos2.x;
	float z = pos1.z - pos2.z;
	distance = { x,0,z,0 };
	return distance;
}
//���K��
inline XMVECTOR Normalize(XMVECTOR vec)
{
	Vector3 ret{};
	ret.x = vec.m128_f32[0];
	ret.y = vec.m128_f32[1];
	ret.z = vec.m128_f32[2];
	ret.normalize();

	XMVECTOR _ret{};
	_ret.m128_f32[0] = ret.x;
	_ret.m128_f32[1] = ret.y;
	_ret.m128_f32[2] = ret.z;
	_ret.m128_f32[3] = 0;
	return _ret;
}

/// <summary>
/// ����n�_����
/// </summary>
/// <param name="pos1"></param>
/// <param name="pos2"></param>
/// <returns></returns>
inline bool samePoint(XMFLOAT3 pos1, XMFLOAT3 pos2) {
	if (pos1.x != pos2.x) { return false; }
	if (pos1.y != pos2.y) { return false; }
	if (pos1.z != pos2.z) { return false; }
	return true;
}

/// <summary>
/// �����x�N�g���̎Z�o
/// </summary>
/// <param name="vec"></param>
/// <returns></returns>
inline XMVECTOR rightVec(XMVECTOR vec) {
	Vector3 tmp = { 1,0,0 };
	Vector3 vec_ = { vec.m128_f32[0],vec.m128_f32[1],vec.m128_f32[2] };

	//��̃x�N�g���̂Ȃ��p���v�Z
	float cos = float((vec_.x * tmp.x + vec_.y * tmp.y + vec_.z * tmp.z)
		/ sqrt(vec_.x * vec_.x + vec_.y * vec_.y + vec_.z * vec_.z)
		* sqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z));

	//�v�Z���x�N�g�����v�Z�p�x�N�g���ƌ���Ȃ������ꍇ
	if (cos < 0.00017f) {
		//�v�Z�p�x�N�g����ύX
		tmp = { 0,1,0 };
	}

	//�O�ςŐ����x�N�g�����Z�o
	Vector3 answer = vec_.cross(tmp);

	return XMVECTOR{ answer.x, answer.z, answer.y, 0 };
}
