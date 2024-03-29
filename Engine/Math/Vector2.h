#pragma once
#include <cmath>
struct Vec2
{
	double x;
	double y;

	Vec2() = default;

	Vec2(double _x, double _y)
		: x(_x)
		, y(_y) {}

	double length() const	// 長さ
	{
		return sqrt(x * x + y * y);
	}

	double lengthSquare() const // 長さの二乗
	{
		return x * x + y * y;
	}

	double dot(const Vec2& a) const // 内積
	{
		return x * a.x + y * a.y;
	}

	double distance(const Vec2& a) const    // 距離
	{
		return sqrt((a.x - x) * (a.x - x) + (a.y - y) * (a.y - y));
	}

	Vec2 normalized() const // 正規化
	{
		double	_len = length();
		return{ x / _len , y / _len };
	}

	bool isZero() const // ゼロベクトルであるか
	{
		return x == 0.0 && y == 0.0;
	}

	void clear(double a)
	{
		x = a;
		y = a;
	}

	Vec2 operator +() const
	{
		return *this;
	}

	Vec2 operator -() const
	{
		return{ -x, -y };
	}

	Vec2 operator +(const Vec2& other) const
	{
		return{ x + other.x, y + other.y };
	}

	Vec2 operator -(const Vec2& other) const
	{
		return{ x - other.x, y - other.y };
	}

	Vec2 operator *(double s) const
	{
		return{ x * s, y * s };
	}

	Vec2 operator /(double s) const
	{
		return{ x / s, y / s };
	}

	Vec2& operator +=(const Vec2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec2& operator -=(const Vec2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec2& operator *=(double s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	Vec2& operator /=(double s)
	{
		x /= s;
		y /= s;
		return *this;
	}
};

inline Vec2 operator *(double s, const Vec2& v) // Vec2 が後にくる 2項 *
{
	return{ s * v.x, s * v.y };
}
