#include "SphereCollider.h"

void SphereCollider::Update()
{
	XMMATRIX matWorld = {};
	if (object3d != nullptr) { matWorld = object3d->GetMatWorld(); }
	else if (fbxObject != nullptr) { matWorld = fbxObject->GetMatWorld(); }
	else if (pmdObject != nullptr) { matWorld = pmdObject->model->GetWorldMat(); }
	Sqhere::center = matWorld.r[3] + offset;
	Sqhere::radius = radius;
}
