#pragma once

#include "BaseCollision.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

class SphereCollider : public BaseCollider, public Sqhere {
private:
	using XMVECTOR = DirectX::XMVECTOR;

public:
	SphereCollider(XMVECTOR offset = { 0,0,0,0 }, float radius = 1.0f) :
		offset(offset),
		radius(radius) {
		shapeType = COLLISIONSHAPE_SQHERE;
	}

	void Update()override;

	inline void SetRaidus(float _radius) { this->radius = _radius; }
	inline void SetOffset(const XMVECTOR& _offset) { this->offset = _offset; }

	inline const XMVECTOR& GetOffset() { return offset; }
	inline float GetRadius() { return radius; }
private:
	XMVECTOR offset;

	float radius;
};