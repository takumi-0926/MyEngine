#include "SphereCollider.h"

void SphereCollider::Update()
{
	XMMATRIX matWorld = {};
	if (object3d != nullptr) { matWorld = object3d->GetMatWorld(); }
	else if (fbxObject != nullptr) { matWorld = fbxObject->GetMatWorld(); }

	Sqhere::center = matWorld.r[3] + offset;
	Sqhere::radius = radius;
}
