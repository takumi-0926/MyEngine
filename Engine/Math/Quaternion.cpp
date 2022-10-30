#include "Quaternion.h"
#include "Matrix4.h"
#include <cmath>

Quaternion quaternion(float x, float y, float z, float w)
{
	//成分を指定してクォータニオンを作成
	Quaternion result{ x,y,z,w };
	return result;
}

Quaternion quaternion(const Vector3& v, float angle)
{
	//任意軸周りの回転の指定からクォータニオンを作成
	float _sin = sin(angle / 2.0f);
	return quaternion(_sin * v.x, _sin * v.y, _sin * v.z, cos(angle / 2.0f));
}

float dot(const Quaternion& q1, const Quaternion& q2)
{
	return	q1.w * q2.w +
		q1.x * q2.x +
		q1.y * q2.y +
		q1.z * q2.z;
}

float length(const Quaternion& q)
{
	return (float)sqrt(dot(q, q));
}

Quaternion normalize(const Quaternion& q)
{
	Quaternion result = q;
	float len = length(result);
	if (len != 0) {
		result /= len;
	}
	return result;
}

Quaternion conjugate(const Quaternion& q)
{
	Quaternion result = { -q.x,-q.y,-q.z,q.w };
	return result;
}

Quaternion operator+(const Quaternion& q)
{
	return q;
}

Quaternion operator-(const Quaternion& q)
{
	Quaternion result = { -q.x,-q.y ,-q.z ,-q.w };
	return result;
}

Quaternion& operator+=(Quaternion& q1, const Quaternion& q2)
{
	q1.x += q2.x;
	q1.y += q2.y;
	q1.z += q2.z;
	q1.w += q2.w;
	return q1;
	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator-=(Quaternion& q1, const Quaternion& q2)
{
	q1.x -= q2.x;
	q1.y -= q2.y;
	q1.z -= q2.z;
	q1.w -= q2.w;
	return q1;
	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator*=(Quaternion& q, float s)
{
	q.x *= s;
	q.y *= s;
	q.z *= s;
	q.w *= s;
	return q;

	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator/=(Quaternion& q, float s)
{
	return q *= 1.0f / s;
	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator*=(Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = {
	q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x,
	-q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y,
	q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z,
	-q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w
	};

	q1 = result;
	return q1;
	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator+(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result += q2;
	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator-(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result -= q2;
	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result *= q2;
	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator*(const Quaternion& q, float s)
{
	Quaternion result = q;
	return result *= s;
	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator*(float s, const Quaternion& q)
{
	Quaternion result = q;
	return result *= s;
	// TODO: return ステートメントをここに挿入します
}

Quaternion& operator/(const Quaternion& q, float s)
{
	Quaternion result = q;
	return result /= s;
	// TODO: return ステートメントをここに挿入します
}

Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float cos = dot(q1, q2);
	Quaternion t2 = q2;
	if (cos < 0.0f) {
		cos = -cos;
		t2 = -q2;
	}
	float k0 = 1.0f - t;
	float k1 = t;
	if ((1.0f - cos) > 0.001f) {
		float theta = (float)acos(cos);
		k0 = (float)(sin(theta * k0) / sin(theta));
		k1 = (float)(sin(theta * k1) / sin(theta));
	}

	Quaternion reQ1 = q1 * k0;
	Quaternion reQ2 = t2 * k1;
	Quaternion reQ3 = reQ1 + reQ2;
	return reQ3;
}

Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float cos = dot(q1, q2);
	Quaternion t2 = q2;
	if (cos < 0.0f) {
		cos = -cos;
		t2 = -q2;
	}
	float k0 = 1.0f - t;
	float k1 = t;
	return q1 * k0 * t2 * k1;
}

Matrix4 rotate(const Quaternion& q)
{
	float xx = q.x * q.x * 2.0f;
	float yy = q.y * q.y * 2.0f;
	float zz = q.z * q.z * 2.0f;
	float xy = q.x * q.y * 2.0f;
	float xz = q.x * q.z * 2.0f;
	float yz = q.y * q.z * 2.0f;
	float wx = q.w * q.x * 2.0f;
	float wy = q.w * q.y * 2.0f;
	float wz = q.w * q.z * 2.0f;
	DirectX::XMMATRIX result = {
		1.0f - yy - zz,		xy + wz,	xz - wy,	0.0f,
		xy - wz,	 1.0f - xx - zz,	yz + wx,	0.0f,
		xz + wy,		yz - wx, 1.0f - xx - yy,	0.0f,
		0.0f,				   0.0f,	   0.0f,	1.0f
	};
	return result;
}

Quaternion quaternion(const Matrix4& m)
{
	Quaternion result;
	float tr = m.r[0].m128_f32[0] + m.r[1].m128_f32[1] + m.r[2].m128_f32[2] + m.r[3].m128_f32[3];

	if (tr >= 1.0f) {
		float fourD = 2.0f * sqrt(tr);
		result.x = (m.r[1].m128_f32[2] - m.r[2].m128_f32[1]) / fourD;
		result.y = (m.r[2].m128_f32[0] - m.r[0].m128_f32[2]) / fourD;
		result.z = (m.r[0].m128_f32[1] - m.r[1].m128_f32[0]) / fourD;
		result.w = fourD / 4.0f;
		return result;
	}

	int i = 0;
	if (m.r[0].m128_f32[0] <= m.r[1].m128_f32[1]) {
		i = 1;
	}
	if (m.r[2].m128_f32[2] > m.r[i].m128_f32[i]) {
		i = 2;
	}
	int j = (i + 1) % 3;
	int k = (j + 1) % 3;
	tr = m.r[i].m128_f32[i] - m.r[j].m128_f32[j] - m.r[k].m128_f32[k] + 1.0f;

	float fourD = 2.0f * sqrt(tr);
	float qa[4];
	qa[i] = fourD / 4.0f;
	qa[j] = (m.r[j].m128_f32[i] + m.r[i].m128_f32[j]) / fourD;
	qa[k] = (m.r[k].m128_f32[i] + m.r[i].m128_f32[k]) / fourD;
	qa[3] = (m.r[j].m128_f32[k] - m.r[k].m128_f32[j]) / fourD;

	result.x = qa[0];
	result.y = qa[1];
	result.z = qa[2];
	result.w = qa[3];
	return result;
}

Vector3 getAxis(const Quaternion& q)
{
	Vector3 result;

	float x = q.x;
	float y = q.x;
	float z = q.x;
	float _len = length(q);

	result.x = x / _len;
	result.y = y / _len;
	result.z = z / _len;

	return result;
}
