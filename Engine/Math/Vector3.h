#pragma once

class Vector3
{
public:
	float x;	//X成分
	float y;	//Y成分
	float z;	//Z成分

	//コンストラクタ
	Vector3();	//ゼロベクトルとして生成
	Vector3(float x, float y,float z);	//X成分,Y成分,Z成分を指定しての生成

	//メンバ関数
	float length()const;	//ノルム（ベクトルの長さ）を求める
	Vector3& normalize();	//正規化する
	float dot(const Vector3& v)const;	//内積を求める
	Vector3 cross(const Vector3& v)const;	//外積を求める

	//単項演算子オーバーロード
	Vector3 operator+()const;
	Vector3 operator-()const;

	//代入演算子オーバーロード
	Vector3& operator+=(const  Vector3& v);
	Vector3& operator-=(const  Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

//2項演算子オーバーロード
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);

const Vector3 lerp(const Vector3& strat, const Vector3& end, const float t);
