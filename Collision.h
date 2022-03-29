#pragma once

#include "CollisionPrimitive.h"

class Coliision {
public:
	/// <summary>
	/// ‹…‚Æ•½–Ê‚Ì“–‚½‚è”»’è
	/// </summary>
	static bool CheckSqhere2Plane(
		const Sqhere& sqhere,
		const Plane& plane,
		XMVECTOR* inter = nullptr);

	static void ClosestPtPoint2Triangle(
		const XMVECTOR& point,
		const Triangle& triangle,
		XMVECTOR* closest);

	static bool CheckSqhere2Triangle(
		const Sqhere& sqhere,
		const Triangle& triangle,
		XMVECTOR* inter = nullptr);

	static bool CheckRay2Plane(
		const Ray& ray,
		const Plane& plane,
		float* distance = nullptr,
		XMVECTOR* inter = nullptr
		);

	static bool CheckRay2Trianlge(
		const Ray& ray,
		const Triangle& triangle,
		float* distance = nullptr,
		XMVECTOR* inter = nullptr);

	static bool CheckRay2Sqhere(
		const Ray& ray,
		const Sqhere& sqhere,
		float* distance = nullptr,
		XMVECTOR* inter = nullptr);
};
