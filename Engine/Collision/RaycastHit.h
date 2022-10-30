#pragma once
#include "BaseCollision.h"
#include <DirectXMath.h>

class Object3Ds;

struct RaycastHit {
	Object3Ds* object = nullptr;

	BaseCollider* collider = nullptr;

	DirectX::XMVECTOR inter;

	float distance = 0.0f;
};