#include "CollisionManager.h"
#include "BaseCollision.h"
#include "Collision.h"

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

			if (colA->GEtShapeType() == COLLISIONSHAPE_SQHERE &&
				colB->GEtShapeType() == COLLISIONSHAPE_SQHERE) {
				Sqhere* SphereA = dynamic_cast<Sqhere*>(colA);
				Sqhere* SphereB = dynamic_cast<Sqhere*>(colB);

				DirectX::XMVECTOR inter;
				if (Collision::CheckSqhere2Sqhere(*SphereA, *SphereB)) {
					colA->OnCllision(CollisionInfo(colA->GetObject3d(), colB, inter));
					colB->OnCllision(CollisionInfo(colB->GetObject3d(), colA, inter));
				}
			}
			else if (colA->GEtShapeType() == COLLISIONSHAPE_MESH &&
				colB->GEtShapeType() == COLLISIONSHAPE_SQHERE) {
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);
				Sqhere* sphere = dynamic_cast<Sqhere*>(colB);
				DirectX::XMVECTOR inter;
				if (meshCollider->CheckCollisionSqhere(*sphere, &inter)) {
					colA->OnCllision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCllision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
			else if (colA->GEtShapeType() == COLLISIONSHAPE_SQHERE &&
				colB->GEtShapeType() == COLLISIONSHAPE_MESH) {
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colB);
				Sqhere* sphere = dynamic_cast<Sqhere*>(colA);
				DirectX::XMVECTOR inter;
				if (meshCollider->CheckCollisionSqhere(*sphere, &inter)) {
					colA->OnCllision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCllision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
		}
	}
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
	XMVECTOR inter;

	it = colliders.begin();
	for (; it != colliders.end(); ++it)
	{
		BaseCollider* colA = *it;

		//属性が合わなかったら飛ばす
		if (!(colA->attribute & attribute)) { continue; }

		//レイと球
		if (colA->GEtShapeType() == COLLISIONSHAPE_SQHERE) {
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
		else if (colA->GEtShapeType() == COLLISIONSHAPE_MESH) {
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
		}
		return false;
	}
}

void CollisionManager::QuerySqhere(const Sqhere& sphere, QueryCallBack* callBack, unsigned short attribute)
{
	assert(callBack);

	std::forward_list<BaseCollider*>::iterator it;

	it = colliders.begin();
	for (; it != colliders.end(); ++it){
		BaseCollider* col = *it;

		if (!(col->attribute & attribute))
		{
			continue;
		}

		if (col->GEtShapeType() == COLLISIONSHAPE_SQHERE) {
			Sqhere* sphereB = dynamic_cast<Sqhere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (Collision::CheckSqhere2Sqhere(sphere, *sphereB, &tempInter, &tempReject))continue;

			//交差情報設定
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callBack->OnQueryHit(info)) {
				return;
			}
		}
		else if (col->GEtShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!meshCollider->CheckCollisionSqhere(sphere, &tempInter, &tempReject))continue;

			//交差情報設定
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callBack->OnQueryHit(info)) {
				return;
			}
		}
	}
}
