#pragma once
#include "CollisionPrimitive.h"
#include "QueryCallBack.h"
#include "RaycastHit.h"

#include <d3d12.h>
#include <forward_list>

class BaseCollider;

class CollisionManager {
public:
	static CollisionManager* GetInstance();

public:
	inline void AddCollider(BaseCollider* collider) {
		colliders.push_front(collider);
	}

	inline void RemoveCollider(BaseCollider* collider) {
		colliders.remove(collider);
	}

	void CheckAllCollision();

	/// <summary>
	/// レイキャスト（属性なし版）
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="hitInfo">衝突情報</param>
	/// <param name="maxDistance">最大距離</param>
	/// <returns></returns>
	bool Raycast(
		const Ray& ray,
		RaycastHit* hitInfo = nullptr,
		float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	/// レイキャスト（属性アリ版）
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="attribute">判定属性</param>
	/// <param name="hitInfo">衝突情報</param>
	/// <param name="maxDistance">最大距離</param>
	/// <returns></returns>
	bool Raycast(
		const Ray& ray,
		unsigned short attribute,
		RaycastHit* hitInfo = nullptr,
		float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	/// 球による衝突全判定
	/// </summary>
	/// <param name="sphere"></param>
	/// <param name="callBack"></param>
	/// <param name="attribute"></param>
	void QuerySqhere(
		const Sqhere& sphere,
		QueryCallBack* callBack,
		unsigned short attribute = (unsigned short)0xffffffff);

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;

	std::forward_list<BaseCollider*> colliders;
};