#pragma once

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

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;

	std::forward_list<BaseCollider*> colliders;
};