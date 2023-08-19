#pragma once
#include <DirectXMath.h>
#include "Math/Matrix4.h"
#include <Math/Quaternion.h>

using namespace DirectX;

extern const float fps;

//XMFLOAT3型関係演算子オーバーロード
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

//スケール抽出
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
//位置抽出
inline XMMATRIX ExtractPositionMat(XMMATRIX matworld)
{
	return XMMatrixTranslation(matworld.r[3].m128_f32[0], matworld.r[3].m128_f32[1], matworld.r[3].m128_f32[2]);
}
//回転抽出
inline XMMATRIX ExtractRotationMat(XMMATRIX matworld)
{
	XMMATRIX mOffset = ExtractPositionMat(matworld);
	XMMATRIX mScaling = ExtractScaleMat(matworld);

	XMVECTOR det;
	// 左からScaling、右からOffsetの逆行列をそれぞれかける。
	return XMMatrixInverse(&det, mScaling) * matworld * XMMatrixInverse(&det, mOffset);
}
//回転
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

//移動
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

//距離
inline float distance(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	float distance{};
	float x = abs(pos1.x - pos2.x);
	float z = abs(pos1.z - pos2.z);
	distance = float(sqrt(x * 2 + z * 2));
	return distance;
}

//Vector型をXMFLAT3型にキャスト
inline XMFLOAT3 VectorToXMFloat(XMVECTOR vec)
{
	XMFLOAT3 ret{};
	ret.x = vec.m128_f32[0];
	ret.y = vec.m128_f32[1];
	ret.z = vec.m128_f32[2];
	return ret;
}

//二点間のベクトルを算出
inline XMVECTOR twoPointVector(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	XMVECTOR distance{};
	float x = pos1.x - pos2.x;
	float z = pos1.z - pos2.z;
	distance = { x,0,z,0 };
	return distance;
}
//正規化
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
/// 同一地点判別
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
/// 垂直ベクトルの算出
/// </summary>
/// <param name="vec"></param>
/// <returns></returns>
inline XMVECTOR rightVec(XMVECTOR vec) {
	Vector3 tmp = { 1,0,0 };
	Vector3 vec_ = { vec.m128_f32[0],vec.m128_f32[1],vec.m128_f32[2] };

	//二つのベクトルのなす角を計算
	float cos = float((vec_.x * tmp.x + vec_.y * tmp.y + vec_.z * tmp.z)
		/ sqrt(vec_.x * vec_.x + vec_.y * vec_.y + vec_.z * vec_.z)
		* sqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z));

	//計算元ベクトルが計算用ベクトルと限りなく同じ場合
	if (cos < 0.00017f) {
		//計算用ベクトルを変更
		tmp = { 0,1,0 };
	}

	//外積で垂直ベクトルを算出
	Vector3 answer = vec_.cross(tmp);

	return XMVECTOR{ answer.x, answer.z, answer.y, 0 };
}
