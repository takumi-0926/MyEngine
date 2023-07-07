#include "OBBCollision.h"
#include <DirectXMath.h>
#include "Math/Vector3.h"

bool collisionOBBtoOBB(OBB& obb1, OBB& obb2)
{
    //•ûŒüƒxƒNƒgƒ‹‚Ìİ’è
    Vector3 normalAVec1 = obb1.GetVector(0);
    Vector3 AVec1= normalAVec1 * obb1.GetLength(0);
    Vector3 normalAVec2 = obb1.GetVector(1);
    Vector3 AVec2 = normalAVec2 * obb1.GetLength(1);
    Vector3 normalAVec3 = obb1.GetVector(2);
    Vector3 AVec3 = normalAVec3 * obb1.GetLength(2);

    Vector3 normalBVec1 = obb2.GetVector(0);
    Vector3 BVec1 = normalBVec1 * obb2.GetLength(0);
    Vector3 normalBVec2 = obb2.GetVector(1);
    Vector3 BVec2 = normalBVec2 * obb2.GetLength(1);
    Vector3 normalBVec3 = obb2.GetVector(2);
    Vector3 BVec3 = normalBVec3 * obb2.GetLength(2);

    Vector3 interval = obb1.GetPos() - obb2.GetPos();

    //A1
    float rA = AVec1.length();
    float rB = LenSegOnSeparateAxis(normalAVec1, BVec1, BVec2, &BVec3);
    float L = float(fabs(interval.dot(normalAVec1)));
    if (L > rA + rB) {
        return false;
    }

    //A2
    rA = AVec2.length();
    rB = LenSegOnSeparateAxis(normalAVec2, BVec1, BVec2, &BVec3);
    L = float(fabs(interval.dot(normalAVec2)));
    if (L > rA + rB) {
        return false;
    }

    //A3
    rA = AVec3.length();
    rB = LenSegOnSeparateAxis(normalAVec3, BVec1, BVec2, &BVec3);
    L = float(fabs(interval.dot(normalAVec3)));
    if (L > rA + rB) {
        return false;
    }

    //B1
    rA = BVec1.length();
    rB = LenSegOnSeparateAxis(normalBVec1, AVec1, AVec2, &AVec3);
    L = float(fabs(interval.dot(normalBVec1)));
    if (L > rA + rB) {
        return false;
    }

    //B2
    rA = BVec2.length();
    rB = LenSegOnSeparateAxis(normalBVec2, AVec1, AVec2, &AVec3);
    L = float(fabs(interval.dot(normalBVec2)));
    if (L > rA + rB) {
        return false;
    }

    //B3
    rA = BVec3.length();
    rB = LenSegOnSeparateAxis(normalBVec3, AVec1, AVec2, &AVec3);
    L = float(fabs(interval.dot(normalAVec3)));
    if (L > rA + rB) {
        return false;
    }

    //C1
    Vector3 Cross = normalAVec1.cross(normalBVec1);
    rA = LenSegOnSeparateAxis(Cross, AVec2, AVec3);
    rB = LenSegOnSeparateAxis(Cross, BVec2, BVec3);
    L = float(fabs(interval.dot(Cross)));
    if (L > rA + rB) {
        return false;
    }

    //C2
    Cross = normalAVec1.cross(normalBVec2);
    rA = LenSegOnSeparateAxis(Cross, AVec2, AVec3);
    rB = LenSegOnSeparateAxis(Cross, BVec1, BVec3);
    L = float(fabs(interval.dot(Cross)));
    if (L > rA + rB) {
        return false;
    }

    //C3
    Cross = normalAVec1.cross(normalBVec3);
    rA = LenSegOnSeparateAxis(Cross, AVec2, AVec3);
    rB = LenSegOnSeparateAxis(Cross, BVec1, BVec2);
    L = float(fabs(interval.dot(Cross)));
    if (L > rA + rB) {
        return false;
    }

    //C21
    Cross = normalAVec2.cross(normalBVec1);
    rA = LenSegOnSeparateAxis(Cross, AVec1, AVec3);
    rB = LenSegOnSeparateAxis(Cross, BVec2, BVec3);
    L = float(fabs(interval.dot(Cross)));
    if (L > rA + rB) {
        return false;
    }

    //C22
    Cross = normalAVec2.cross(normalBVec2);
    rA = LenSegOnSeparateAxis(Cross, AVec1, AVec3);
    rB = LenSegOnSeparateAxis(Cross, BVec1, BVec3);
    L = float(fabs(interval.dot(Cross)));
    if (L > rA + rB) {
        return false;
    }

    //C23
    Cross = normalAVec2.cross(normalBVec3);
    rA = LenSegOnSeparateAxis(Cross, AVec1, AVec3);
    rB = LenSegOnSeparateAxis(Cross, BVec1, BVec2);
    L = float(fabs(interval.dot(Cross)));
    if (L > rA + rB) {
        return false;
    }

    //C31
    Cross = normalAVec3.cross(normalBVec1);
    rA = LenSegOnSeparateAxis(Cross, AVec1, AVec1);
    rB = LenSegOnSeparateAxis(Cross, BVec2, BVec3);
    L = float(fabs(interval.dot(Cross)));
    if (L > rA + rB) {
        return false;
    }

    //C32
    Cross = normalAVec3.cross(normalBVec2);
    rA = LenSegOnSeparateAxis(Cross, AVec1, AVec2);
    rB = LenSegOnSeparateAxis(Cross, BVec1, BVec3);
    L = float(fabs(interval.dot(Cross)));
    if (L > rA + rB) {
        return false;
    }

    //C33
    Cross = normalAVec3.cross(normalBVec3);
    rA = LenSegOnSeparateAxis(Cross, AVec1, AVec2);
    rB = LenSegOnSeparateAxis(Cross, BVec1, BVec2);
    L = float(fabs(interval.dot(Cross)));
    if (L > rA + rB) {
        return false;
    }

    return true;
}

// •ª—£²‚É“Š‰e‚³‚ê‚½²¬•ª‚©‚ç“Š‰eü•ª’·‚ğZo
float LenSegOnSeparateAxis(Vector3& Sep, Vector3& e1, Vector3& e2, Vector3* e3)
{
    // 3‚Â‚Ì“àÏ‚Ìâ‘Î’l‚Ì˜a‚Å“Š‰eü•ª’·‚ğŒvZ
    // •ª—£²Sep‚Í•W€‰»‚³‚ê‚Ä‚¢‚é‚±‚Æ
    float r1 = float(fabs(Sep.dot(e1)));
    float r2 = float(fabs(Sep.dot(e2)));
    float r3 = e3 ? (float(fabs(Sep.dot(*e3)))) : 0;
    return r1 + r2 + r3;
}