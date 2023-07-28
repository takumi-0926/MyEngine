#pragma once
#include "OBB.h"

class OBB;

bool collisionOBBtoOBB(OBB& obb1, OBB& obb2);

float LenSegOnSeparateAxis(Vector3& Sep, Vector3& e1, Vector3& e2, Vector3* e3 = 0);

const DirectX::XMVECTOR operator* (const DirectX::XMVECTOR& v, float s);