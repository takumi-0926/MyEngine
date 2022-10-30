#pragma once

#include<DirectXMath.h>

class Object3Ds;
class BaseCollider;

/// <summary>
/// 
/// </summary>
struct QueryHit {
	Object3Ds* object = nullptr;

	BaseCollider* collider = nullptr;

	DirectX::XMVECTOR inter;

	DirectX::XMVECTOR reject;
};

/// <summary>
/// 
/// </summary>
class QueryCallBack {
public:
	QueryCallBack() = default;
	virtual ~QueryCallBack() = default;

	virtual bool OnQueryHit(const QueryHit& info) = 0;
};