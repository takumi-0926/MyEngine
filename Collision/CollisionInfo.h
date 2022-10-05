#pragma once

#include <DirectXMath.h>

class Object3Ds;
class BaseCollider;

/// <summary>
/// 衝突情報
/// </summary>
struct CollisionInfo {
public:
	CollisionInfo(
		Object3Ds* _object,
		BaseCollider* _collider, 
		const DirectX::XMVECTOR& _inter) {
		this->object = _object;
		this->collider = _collider;
		this->inter = _inter;
	}

	//衝突相手のオブジェクト
	Object3Ds* object = nullptr;
	//衝突相手のコライダー
	BaseCollider* collider = nullptr;
	//衝突点
	DirectX::XMVECTOR inter;
};