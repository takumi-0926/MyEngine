#include "CollisionManager.h"
#include "BaseCollision.h"
#include "Collision.h"
#include "CollisionAttribute.h"
#include "MeshCollider.h"

CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::CheckAllCollision()
{
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;

	itA = colliders.begin();
	for (; itA != colliders.end(); ++itA) {
		itB = itA;
		++itB;

		for (; itB != colliders.end(); ++itB)
		{
			BaseCollider* colA = *itA;
			BaseCollider* colB = *itB;

			if (colA->GetShapeType() == COLLISIONSHAPE_SQHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_SQHERE) {
				Sqhere* SphereA = dynamic_cast<Sqhere*>(colA);
				Sqhere* SphereB = dynamic_cast<Sqhere*>(colB);

				DirectX::XMVECTOR inter;
				if (Collision::CheckSqhere2Sqhere(*SphereA, *SphereB)) {
					colA->OnCllision(CollisionInfo(colB->GetObject3d(), colB->GetFbxObject3d(), colB->GetPmdObject3d(), colB, inter));
					colB->OnCllision(CollisionInfo(colA->GetObject3d(), colA->GetFbxObject3d(), colA->GetPmdObject3d(), colA, inter));
				}
			}
			else if (colA->GetShapeType() == COLLISIONSHAPE_SQHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_TRYANGLE) {
				Sqhere* sphere = dynamic_cast<Sqhere*>(colA);
				Triangle* triangle = dynamic_cast<Triangle*>(colB);

				DirectX::XMVECTOR inter;
				if (Collision::CheckSqhere2Triangle(*sphere, *triangle)) {
					colA->OnCllision(CollisionInfo(colB->GetObject3d(), colB->GetFbxObject3d(), colB->GetPmdObject3d(), colB, inter));
					colB->OnCllision(CollisionInfo(colA->GetObject3d(), colA->GetFbxObject3d(), colA->GetPmdObject3d(), colA, inter));
				}
			}
			else if (colA->GetShapeType() == COLLISIONSHAPE_MESH &&
				colB->GetShapeType() == COLLISIONSHAPE_SQHERE) {
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);
				Sqhere* sphere = dynamic_cast<Sqhere*>(colB);
				DirectX::XMVECTOR inter;
				if (meshCollider->CheckCollisionSqhere(*sphere, &inter)) {
					colA->OnCllision(CollisionInfo(colB->GetObject3d(), colB->GetFbxObject3d(), colB->GetPmdObject3d(), colB, inter));
					colB->OnCllision(CollisionInfo(colA->GetObject3d(), colA->GetFbxObject3d(), colA->GetPmdObject3d(), colA, inter));
				}
			}
			else if (colA->GetShapeType() == COLLISIONSHAPE_SQHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_MESH) {
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colB);
				Sqhere* sphere = dynamic_cast<Sqhere*>(colA);
				DirectX::XMVECTOR inter;
				if (meshCollider->CheckCollisionSqhere(*sphere, &inter)) {
					colA->OnCllision(CollisionInfo(colB->GetObject3d(), colB->GetFbxObject3d(), colB->GetPmdObject3d(), colB, inter));
					colB->OnCllision(CollisionInfo(colA->GetObject3d(), colA->GetFbxObject3d(), colA->GetPmdObject3d(), colA, inter));
				}
			}
		}
	}
}

bool CollisionManager::Raycast(
	const Ray& ray,
	RaycastHit* hitInfo,
	float maxDistance)
{
	return Raycast(ray, 0xffff, hitInfo, maxDistance);
}

bool CollisionManager::Raycast(
	const Ray& ray,
	unsigned short attribute,
	RaycastHit* hitInfo,
	float maxDistance)
{
	bool result = false;
	std::forward_list<BaseCollider*>::iterator it;
	std::forward_list<BaseCollider*>::iterator it_hit;
	float distance = maxDistance;
	XMVECTOR inter = {};

	it = colliders.begin();
	for (; it != colliders.end(); ++it)
	{
		BaseCollider* colA = *it;

		//属性が合わなかったら飛ばす
		if (!(colA->attribute & attribute)) { continue; }

		//レイと球
		if (colA->GetShapeType() == COLLISIONSHAPE_SQHERE) {
			Sqhere* sphere = dynamic_cast<Sqhere*>(colA);

			float tempDistance;
			XMVECTOR tempInter;
			if (!Collision::CheckRay2Sqhere(ray, *sphere, &tempDistance, &tempInter))continue;

			if (tempDistance >= distance)continue;

			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
		//レイとメッシュ
		else if (colA->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);

			float tempDistance;
			DirectX::XMVECTOR tempInter;
			if (!meshCollider->CheckCollisionRay(ray, &tempDistance, &tempInter))continue;
			if (tempDistance >= distance)continue;

			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}

		if (result && hitInfo) {
			hitInfo->distance = distance;
			hitInfo->inter = inter;
			hitInfo->collider = *it_hit;
			hitInfo->object = hitInfo->collider->GetObject3d();
			hitInfo->fbx = hitInfo->collider->GetFbxObject3d();
		}
		return result;
	}
	return result;
}

void CollisionManager::QuerySqhere(const Sqhere& sphere, QueryCallBack* callBack, unsigned short attribute, unsigned short MyNumber)
{
	assert(callBack);

	std::forward_list<BaseCollider*>::iterator it;

	//全てのコライダーと総当たりチェック
	it = colliders.begin();
	for (; it != colliders.end(); ++it) {
		BaseCollider* col = *it;

		//属性が合わなければスキップ
		if (!(col->attribute & attribute) || col->attribute == COLLISION_ATTR_BULLET) {
			//自分でなければスキップしない
			if (!(col->MyNumber & MyNumber)) {
				continue;
			}
		}

		//球の場合
		if (col->GetShapeType() == COLLISIONSHAPE_SQHERE) {
			Sqhere* sphereB = dynamic_cast<Sqhere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!Collision::CheckSqhere2Sqhere(sphere, *sphereB, &tempInter, &tempReject)) {
				continue;
			}
			//交差情報設定
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.fbx = col->GetFbxObject3d();
			info.pmd = col->GetPmdObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callBack->OnQueryHit(info)) {
				return;
			}
		}
		//三角形の場合
		if (col->GetShapeType() == COLLISIONSHAPE_TRYANGLE) {
			Triangle* triangle = dynamic_cast<Triangle*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!Collision::CheckSqhere2Triangle(sphere, *triangle, &tempInter, &tempReject)) {
				continue;
			}
			//交差情報設定
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.fbx = col->GetFbxObject3d();
			info.pmd = col->GetPmdObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callBack->OnQueryHit(info)) {
				return;
			}
		}
		//メッシュの場合
		else if (col->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!meshCollider->CheckCollisionSqhere(sphere, &tempInter, &tempReject))continue;

			//交差情報設定
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.fbx = col->GetFbxObject3d();
			info.pmd = col->GetPmdObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callBack->OnQueryHit(info)) {
				return;
			}
		}
	}
}
