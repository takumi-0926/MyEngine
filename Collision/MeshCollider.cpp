#include "MeshCollider.h"
#include "Collision.h"

using namespace DirectX;
void MeshCollider::ConstructTriangles(Model* model)
{
	triangles.clear();
	const std::vector<Mesh*>& meshes = model->GetMesh();

	int start = 0;

	std::vector<Mesh*>::const_iterator it = meshes.cbegin();
	for (; it != meshes.cend(); ++it) {
		Mesh* mesh = *it;
		const std::vector<Mesh::VertexPosNormalUv>& vertices = mesh->GetVertex();
		const std::vector<unsigned short>& indices = mesh->GetIndex();

		size_t triangleNum = indices.size() / 3;

		triangles.resize(triangles.size() + triangleNum);

		for (int i = 0; i < triangleNum; i++)
		{
			Triangle& tri = triangles[start + i];
			int idx0 = indices[i * 3 + 0];
			int idx1 = indices[i * 3 + 1];
			int idx2 = indices[i * 3 + 2];

			tri.p0 = { vertices[idx0].pos.x,vertices[idx0].pos.y,vertices[idx0].pos.z,1 };
			tri.p1 = { vertices[idx1].pos.x,vertices[idx1].pos.y,vertices[idx1].pos.z,1 };
			tri.p2 = { vertices[idx2].pos.x,vertices[idx2].pos.y,vertices[idx2].pos.z,1 };

			//三頂点目から法線を計算
			tri.ComputeNormal();
		}

		start += (int)triangleNum;
	}
}

void MeshCollider::Update()
{
	invMatWorld = XMMatrixInverse(nullptr, GetObject3d()->GetMatWorld());
}

bool MeshCollider::CheckCollisionSqhere(
	const Sqhere& sphere,
	DirectX::XMVECTOR* inter,
	DirectX::XMVECTOR* reject)
{
	//ローカル座標系での球を得る
	Sqhere localSphere;
	localSphere.center = XMVector3Transform(sphere.center, invMatWorld);
	localSphere.radius = XMVector3Length(invMatWorld.r[0]).m128_f32[0];

	//ローカル座標系で交差を確認
	std::vector<Triangle>::const_iterator it = triangles.cbegin();

	for (; it != triangles.cend(); ++it) {
		const Triangle& triangle = *it;

		//球と三角形の当たり判定
		if (Collision::CheckSqhere2Triangle(localSphere, triangle, inter, reject))
		{
			if (inter)
			{
				const XMMATRIX& matWorld = GetObject3d()->GetMatWorld();
				//ワールド座標系の交点を得る
				*inter = XMVector3Transform(*inter, matWorld);
			}
			if (reject)
			{
				//ワールド座標系での排斥ベクトルに変換
				const XMMATRIX& matWorld= GetObject3d()->GetMatWorld();
				*reject = XMVector3TransformNormal(*reject, matWorld);
			}
			return true;
		}
	}
	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& ray, float* distance, DirectX::XMVECTOR* inter)
{
	//ローカル座標系での球を得る
	Ray localRay;
	localRay.start = XMVector3Transform(ray.start, invMatWorld);
	localRay.dir = XMVector3TransformNormal(ray.dir, invMatWorld);

	//ローカル座標系で交差を確認
	std::vector<Triangle>::const_iterator it = triangles.cbegin();

	for (; it != triangles.cend(); ++it) {
		const Triangle& triangle = *it;
		XMVECTOR tempInter;
		//球と三角形の当たり判定
		if (Collision::CheckRay2Trianlge(
			localRay, triangle, nullptr, &tempInter))
		{
			const XMMATRIX& matWorld = GetObject3d()->GetMatWorld();
			//ワールド座標系の交点を得る
			tempInter = XMVector3Transform(tempInter, matWorld);

			//交点とレイ始点の距離を計算
			if (distance) {
				XMVECTOR sub = tempInter - ray.start;
				*distance = XMVector3Dot(sub, ray.dir).m128_f32[0];
			}

			if (inter) {
				*inter = tempInter;
			}
			return true;
		}
	}
	return false;
}
