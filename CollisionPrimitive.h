#pragma once

#include <DirectXMath.h>

using namespace DirectX;
struct Sqhere {
	XMVECTOR center = { 0,0,0,1 };

	float radius = 1.0f;
};

struct Plane {
	XMVECTOR normal = { 0,1,0,0 };

	float distance = 0.0f;
};

struct Ray {
	XMVECTOR start = { 0,0,0,1 };

	XMVECTOR dir = { 1,0,0,0 };
};

class Triangle {
public:
	XMVECTOR p0;
	XMVECTOR p1;
	XMVECTOR p2;

	XMVECTOR normal;

	void ComputeNormal();
};