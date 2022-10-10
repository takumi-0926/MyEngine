#pragma once
#include "BaseCollision.h"
#include "CollisionPrimitive.h"

#include<DirectXMath.h>

class MeshCollider : public BaseCollider {
public:
	MeshCollider() {
		shapeType = COLLISIONSHAPE_MESH;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="model"></param>
	void ConstructTriangles(Model* model);

	/// <summary>
	/// 
	/// </summary>
	void Update()override;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="sqhere"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	bool CheckCollisionSqhere(
		const Sqhere& sphere,
		DirectX::XMVECTOR* inter = nullptr,
		DirectX::XMVECTOR* reject = nullptr);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="ray"></param>
	/// <param name="distance"></param>
	/// <param name="inter"></param>
	/// <returns></returns>
	bool CheckCollisionRay(
		const Ray& ray, 
		float* distance = nullptr,
		DirectX::XMVECTOR *inter = nullptr);

private:

	std::vector<Triangle> triangles;
	//ワールド行列の逆行列
	DirectX::XMMATRIX invMatWorld;
};