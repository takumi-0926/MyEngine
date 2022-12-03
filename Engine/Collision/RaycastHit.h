#pragma once
#include "BaseCollision.h"
#include <DirectXMath.h>

class Object3Ds;
class FbxObject3d;

struct RaycastHit {
	Object3Ds* object = nullptr;

	FbxObject3d* fbx = nullptr;

	BaseCollider* collider = nullptr;

	DirectX::XMVECTOR inter;

	float distance = 0.0f;
};