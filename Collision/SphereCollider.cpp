#include "SphereCollider.h"

void SphereCollider::Update()
{
	const XMMATRIX& matWorld = object3d->GetMatWorld();

	Sqhere::center = matWorld.r[3] + offset;
	Sqhere::radius = radius;
}
