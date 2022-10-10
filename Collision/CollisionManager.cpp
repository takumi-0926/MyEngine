#include "CollisionManager.h"
#include "BaseCollision.h"
#include "Collision.h"

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

		for (;itB != colliders.end(); ++itB)
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
		}
	}
}
