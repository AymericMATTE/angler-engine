#include "pch.h"
#include "physics.h"

namespace angler {

    using namespace DirectX;

// TODO: Move in a utils file
float Clamp(float _val, float _min, float _max){
    float result = _val;
    if (result < _min) _val = _min;
    else if (result > _max) _val = _max;
    return result;
}

XMFLOAT3 Physics::closestFromAABB(XMFLOAT3 _point, AABB _aabb)
{
    XMFLOAT3 _closest;
    XMVECTOR _closestV = XMVectorSet(Clamp(_point.x, _aabb.min.x, _aabb.max.x), 
        Clamp(_point.y, _aabb.min.y, _aabb.max.y), 
        Clamp(_point.z, _aabb.min.z, _aabb.max.z),
        0);

    XMStoreFloat3(&_closest, _closestV);
    return _closest;
}

XMFLOAT3 Physics::ClosestFromOBB(XMFLOAT3 p, OBB b)
{
    XMVECTOR centerVect = XMLoadFloat3(&b.center.xmF);
    XMVECTOR pointVect = XMLoadFloat3(&p);
    XMVECTOR diff = XMVectorSubtract(pointVect, centerVect);

    // Start result at center of box; make steps from there
    XMVECTOR closest = XMLoadFloat3(&b.center.xmF);
    // For each OBB axis...
    for (int i = 0; i < 3; i++) {
        // ...project d onto that axis to get the distance
        // along the axis of d from the box center
        float dist;
        XMStoreFloat(&dist, XMVector3Dot(diff, XMLoadFloat3(&b.axes[i].xmF)));
        // If distance farther than the box extents, clamp to the box
        if (dist > b.extents.arr[i]) dist = b.extents.arr[i];
        if (dist < -b.extents.arr[i]) dist = -b.extents.arr[i];
        // Step that distance along the axis to get world coordinate
        closest = XMVectorAdd(closest, XMVectorSet(dist * b.axes[i].xmF.x, dist * b.axes[i].xmF.y, dist * b.axes[i].xmF.z, 0));
    }

    XMFLOAT3 res;
    XMStoreFloat3(&res, closest);
    return res;
}

float Physics::sqDistFromAABB(XMFLOAT3 _point, AABB _aabb)
{
    float sqDist = 0.0f;

    // For each axis count any excess distance outside box extents
    float v = _point.x;
    if (v < _aabb.min.x) sqDist += (_aabb.min.x - v) * (_aabb.min.x - v);
    if (v > _aabb.max.x) sqDist += (v - _aabb.max.x) * (v - _aabb.max.x);

    v = _point.y;
    if (v < _aabb.min.y) sqDist += (_aabb.min.y - v) * (_aabb.min.y - v);
    if (v > _aabb.max.y) sqDist += (v - _aabb.max.y) * (v - _aabb.max.y);

    v = _point.z;
    if (v < _aabb.min.z) sqDist += (_aabb.min.z - v) * (_aabb.min.z - v);
    if (v > _aabb.max.z) sqDist += (v - _aabb.max.z) * (v - _aabb.max.z);

    return sqDist;
}

SphereBounds Physics::sphereFromAABB(AABB* _aabb) {
    SphereBounds sphere;

    XMVECTOR min = XMLoadFloat3(&_aabb->min);
    XMVECTOR max = XMLoadFloat3(&_aabb->max);
    XMVECTOR mid = XMVectorScale(XMVectorAdd(max, min), 0.5f);

    XMStoreFloat3(&sphere.center, mid); // Set center as average of min and max

    XMFLOAT3 radiusVec;
    XMStoreFloat3(&radiusVec, XMVectorSqrt(XMVector3Dot(XMVectorSubtract(max, mid), XMVectorSubtract(max, mid))));

    sphere.radius =  radiusVec.x;
    return sphere;
}

SphereBounds Physics::sphereFromOBB(OBB* _obb)
{
    SphereBounds sphere;
    sphere.center = _obb->center.xmF;

    sphere.radius = fmax(_obb->extents.arr[2], fmax(_obb->extents.arr[0], _obb->extents.arr[1])); // Get longest of 3 extents

    return sphere;
}

// The isOnFrustum and isOnPlane function were done following the instructions from this site : https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
bool Physics::isOnFrustum(AABB& _aabb, const Frustum& frustum, Transform& transform)
{
    XMVECTOR max = XMLoadFloat3(&_aabb.max);
    XMVECTOR min = XMLoadFloat3(&_aabb.min);
    XMVECTOR center = XMVectorScale(XMVectorAdd(max, min), 0.5f);

    XMFLOAT4X4 world = transform.getWorldMatrix();
    XMMATRIX worldMatrix = XMLoadFloat4x4(&world);

    //Get global scale thanks to our transform
    XMFLOAT3 globalCenter;
    XMStoreFloat3(&globalCenter, XMVector3Transform(center, worldMatrix));

    XMFLOAT3 scale = transform.getScale();

    XMFLOAT3 halfExtents;
    XMStoreFloat3(&halfExtents, XMVectorMultiply(XMVectorSubtract(max, center), XMLoadFloat3(&scale)));

    XMFLOAT3 right =  transform.getRight();
    XMFLOAT3 up =  transform.getUp();
    XMFLOAT3 forward =  transform.getForward();

    const float newIi = std::abs(right.x) * halfExtents.x +std::abs(up.x)* halfExtents.y + std::abs(forward.x) * halfExtents.z;

    const float newIj = std::abs(right.y) * halfExtents.x + std::abs(up.y) * halfExtents.y + std::abs(forward.y) * halfExtents.z;

    const float newIk = std::abs(right.z) * halfExtents.x + std::abs(up.z) * halfExtents.y + std::abs(forward.z) * halfExtents.z;

    //We not need to divise scale because it's based on the half extention of the AABB
    const AABB globalAABB(globalCenter, { newIi, newIj, newIk });

    return (isOnPlane(globalAABB, frustum.leftFace) &&
        isOnPlane(globalAABB, frustum.rightFace) &&
        isOnPlane(globalAABB, frustum.topFace) &&
        isOnPlane(globalAABB, frustum.bottomFace) &&
        isOnPlane(globalAABB, frustum.nearFace) &&
        isOnPlane(globalAABB, frustum.farFace));
}

bool Physics::isOnFrustum(SphereBounds& _sphere, const Frustum& frustum, Transform& transform)
{
    //Get global scale is computed by doing the magnitude of
    //X, Y and Z model matrix's column.
    const XMFLOAT3 globalScale = transform.getScale();

    //Get our global center with process it with the global model matrix of our transform

    XMFLOAT4X4 world = transform.getWorldMatrix();

    XMFLOAT3 globalCenter;
    XMStoreFloat3(&globalCenter, XMVector3Transform(XMLoadFloat3(&_sphere.center), XMLoadFloat4x4(&world)));

    //To wrap correctly our shape, we need the maximum scale scalar.
    const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

    SphereBounds globalSphere(globalCenter, _sphere.radius * maxScale);

    //Check Firstly the result that have the most chance
    //to failure to avoid to call all functions.
    return (isOnPlane(globalSphere, frustum.leftFace) &&
            isOnPlane(globalSphere, frustum.rightFace) &&
            isOnPlane(globalSphere, frustum.farFace) &&
            isOnPlane(globalSphere, frustum.nearFace) &&
            isOnPlane(globalSphere, frustum.topFace) &&
            isOnPlane(globalSphere, frustum.bottomFace));
}

bool Physics::isOnPlane(const AABB& _aabb, const Plane& _plane)
{
    XMVECTOR max = XMLoadFloat3(&_aabb.max);
    XMVECTOR min = XMLoadFloat3(&_aabb.min);
    XMVECTOR vCenter = XMVectorScale(XMVectorAdd(max, min), 0.5f);
    XMFLOAT3 center;
    XMStoreFloat3(&center, vCenter);

    XMFLOAT3 extents;
    XMStoreFloat3(&extents, XMVectorSubtract(max, vCenter));

    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = extents.x * std::abs(_plane.normal.x) +
        extents.y * std::abs(_plane.normal.y) + extents.z * std::abs(_plane.normal.z);

    return -r <= _plane.getSignedDistanceToPlane(center);
}

bool Physics::isOnPlane(const SphereBounds& _sphere, const Plane& _plane)
{
    return _plane.getSignedDistanceToPlane(_sphere.center) >= -_sphere.radius;
}

bool Physics::intersects(SphereBounds& a, SphereBounds& b) {
    XMVECTOR vecA = XMLoadFloat3(&a.center);
    XMVECTOR vecB = XMLoadFloat3(&b.center);

    XMVECTOR diff = XMVectorSubtract(vecA, vecB);
    diff = XMVector3Length(diff); // Splats length on vector

    float length;
    XMStoreFloat(&length, diff);
    return length < a.radius + b.radius;
}

bool Physics::intersects(AABB& a, SphereBounds& b) {
    float sqDist = sqDistFromAABB(b.center, a);
    return sqDist <= b.radius * b.radius;
}

bool Physics::intersects(SphereBounds& a, AABB& b) {
    return Physics::intersects(b, a);
}

bool Physics::intersects(AABB& a, AABB& b){
    XMVECTOR vecMinA = XMLoadFloat3(&a.min);
    XMVECTOR vecMaxB = XMLoadFloat3(&b.max);

    // Exclude if min a is greater than max b on any axis
    if (XMVector3Less(vecMinA, vecMaxB) == false) return false;

    XMVECTOR vecMaxA = XMLoadFloat3(&a.max);
    XMVECTOR vecMinB = XMLoadFloat3(&b.min);

    return DirectX::XMVector3Less(vecMinB, vecMaxA);
}

bool Physics::intersects(OBB& a, OBB& b)
{
    float rA;
    float rB;

    XMFLOAT4X4 r;
    XMFLOAT4X4 rAbs;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            XMVECTOR v = XMVector3Dot(XMLoadFloat3(&a.axes[i].xmF), XMLoadFloat3(&b.axes[j].xmF));
            float dot;
            XMStoreFloat(&dot, v);
            r.m[i][j] = dot;
        }
    }

    Vector3 translation;
    XMVECTOR translationVec = XMVectorSubtract(XMLoadFloat3(&b.center.xmF), XMLoadFloat3(&a.center.xmF));

    float tX, tY, tZ;
    XMStoreFloat(&tX,XMVector3Dot(translationVec, XMLoadFloat3(&a.axes[0].xmF)));
    XMStoreFloat(&tY,XMVector3Dot(translationVec, XMLoadFloat3(&a.axes[1].xmF)));
    XMStoreFloat(&tZ,XMVector3Dot(translationVec, XMLoadFloat3(&a.axes[2].xmF)));
    translationVec = XMVectorSet(tX, tY, tZ, 0);

    XMStoreFloat3(&translation.xmF, translationVec);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            rAbs.m[i][j] = abs(r.m[i][j]) + EPSILON;
        }
    }

    // PRIMARY AXES TESTS //
    // Test axes L = A0, L = A1, L = A2
    for (int i = 0; i < 3; i++) {
        rA = a.extents.arr[i];
        XMStoreFloat(&rB, XMVectorSum(XMVectorMultiply(XMLoadFloat3(&b.extents.xmF), XMLoadFloat4x4(&r).r[i])));
        if (abs(translation.arr[i]) > rA + rB) return 0;
    }
    // Test axes L = B0, L = B1, L = B2
    for (int i = 0; i < 3; i++) {
        rA = a.extents.xmF.x * rAbs.m[0][i] + a.extents.xmF.y * rAbs.m[1][i] + a.extents.xmF.z * rAbs.m[2][i]; // Column so i don't know if it's optimized to transpose and use simd
        rB = b.extents.arr[i];
        if (abs(translation.xmF.x * r.m[0][i] + translation.xmF.y * r.m[1][i] + translation.xmF.z * r.m[2][i]) > rA + rB) return 0;
    }

    // SECONDARY AXIS TESTS //
    // Test axis L = A0 x B0
    rA = a.extents.arr[1] * rAbs.m[2][0] + a.extents.arr[2] * rAbs.m[1][0];
    rB = b.extents.arr[1] * rAbs.m[0][2] + b.extents.arr[2] * rAbs.m[0][1];
    if (abs(translation.arr[2] * r.m[1][0] - translation.arr[1] * r.m[2][0]) > rA + rB) return 0;
    // Test axis L = A0 x B1
    rA = a.extents.arr[1] * rAbs.m[2][1] + a.extents.arr[2] * rAbs.m[1][1];
    rB = b.extents.arr[0] * rAbs.m[0][2] + b.extents.arr[2] * rAbs.m[0][0];
    if (abs(translation.arr[2] * r.m[1][1] - translation.arr[1] * r.m[2][1]) > rA + rB) return 0;
    // Test axis L = A0 x B2
    rA = a.extents.arr[1] * rAbs.m[2][2] + a.extents.arr[2] * rAbs.m[1][2];
    rB = b.extents.arr[0] * rAbs.m[0][1] + b.extents.arr[1] * rAbs.m[0][0];
    if (abs(translation.arr[2] * r.m[1][2] - translation.arr[1] * r.m[2][2]) > rA + rB) return 0;
    // Test axis L = A1 x B0
    rA = a.extents.arr[0] * rAbs.m[2][0] + a.extents.arr[2] * rAbs.m[0][0];
    rB = b.extents.arr[1] * rAbs.m[1][2] + b.extents.arr[2] * rAbs.m[1][1];
    if (abs(translation.arr[0] * r.m[2][0] - translation.arr[2] * r.m[0][0]) > rA + rB) return 0;
    // Test axis L = A1 x B1
    rA = a.extents.arr[0] * rAbs.m[2][1] + a.extents.arr[2] * rAbs.m[0][1];
    rB = b.extents.arr[0] * rAbs.m[1][2] + b.extents.arr[2] * rAbs.m[1][0];
    if (abs(translation.arr[0] * r.m[2][1] - translation.arr[2] * r.m[0][1]) > rA + rB) return 0;
    // Test axis L = A1 x B2
    rA = a.extents.arr[0] * rAbs.m[2][2] + a.extents.arr[2] * rAbs.m[0][2];
    rB = b.extents.arr[0] * rAbs.m[1][1] + b.extents.arr[1] * rAbs.m[1][0];
    if (abs(translation.arr[0] * r.m[2][2] - translation.arr[2] * r.m[0][2]) > rA + rB) return 0;
    // Test axis L = A2 x B0
    rA = a.extents.arr[0] * rAbs.m[1][0] + a.extents.arr[1] * rAbs.m[0][0];
    rB = b.extents.arr[1] * rAbs.m[2][2] + b.extents.arr[2] * rAbs.m[2][1];
    if (abs(translation.arr[1] * r.m[0][0] - translation.arr[0] * r.m[1][0]) > rA + rB) return 0;
    // Test axis L = A2 x B1
    rA = a.extents.arr[0] * rAbs.m[1][1] + a.extents.arr[1] * rAbs.m[0][1];
    rB = b.extents.arr[0] * rAbs.m[2][2] + b.extents.arr[2] * rAbs.m[2][0];
    if (abs(translation.arr[1] * r.m[0][1] - translation.arr[0] * r.m[1][1]) > rA + rB) return 0;
    // Test axis L = A2 x B2
    rA = a.extents.arr[0] * rAbs.m[1][2] + a.extents.arr[1] * rAbs.m[0][2];
    rB = b.extents.arr[0] * rAbs.m[2][1] + b.extents.arr[1] * rAbs.m[2][0];
    if (abs(translation.arr[1] * r.m[0][2] - translation.arr[0] * r.m[1][2]) > rA + rB) return 0;

    // Since no separating axis is found, the OBBs must be intersecting
    return 1;
}

bool Physics::intersects(OBB& a, SphereBounds& b)
{
    XMFLOAT3 closest = ClosestFromOBB(b.center, a);
    XMVECTOR v = XMVectorSubtract(XMLoadFloat3(&closest), XMLoadFloat3(&b.center));

    float distance;
    XMStoreFloat(&distance, XMVector3Dot(v, v));
    return distance <= b.radius * b.radius;
}

bool Physics::intersects(SphereBounds& a, OBB& b)
{
    return Physics::intersects(b, a);
}

AABB::AABB(const XMFLOAT3& _center, const XMFLOAT3& _halfExtents)
{
    //center{ (max + min) * 0.5f },
    //extents{ max.x - center.x, max.y - center.y, max.z - center.z }
    XMStoreFloat3(&min, XMVectorSubtract(XMLoadFloat3(&_center), XMLoadFloat3(&_halfExtents)));
    XMStoreFloat3(&max, XMVectorAdd(XMLoadFloat3(&_center), XMLoadFloat3(&_halfExtents)));
}

void AABB::translate(XMFLOAT3 _by)
{
    XMVECTOR vecMin = XMLoadFloat3(&min);
    XMVECTOR vecMax = XMLoadFloat3(&max);
    XMVECTOR vecTranslation = XMLoadFloat3(&_by);
    XMStoreFloat3(&min, XMVectorAdd(vecMin, vecTranslation));
    XMStoreFloat3(&max, XMVectorAdd(vecMax, vecTranslation));
}

GameBounds::GameBounds(const OBB& _box)
{
    box = new OBB(_box);
    type = BOX;
}
GameBounds::GameBounds(OBB* _box): box(_box)
{
    type = BOX;
}

GameBounds::GameBounds(const SphereBounds& _sphere)
{
    sphere = new SphereBounds(_sphere);
    type = SPHERE;
}

GameBounds::GameBounds(SphereBounds* _sphere): sphere(_sphere)
{
    type = SPHERE;
}

GameBounds::~GameBounds()
{
    if(sphere) delete sphere;
    if(box)    delete box;
}

//void GameBounds::translate(XMFLOAT3 _by){
//
//    if (type == SPHERE) {
//        sphere->translate(_by);
//        // No need to update sphere bounds as it is the same as the one in grid
//    }
//    
//    else {
//        box->translate(_by);
//        dirty = true; // Next time grid is used, update object bounding sphere
//    }
//}

OBB::OBB(XMFLOAT3 _center, XMFLOAT3 _axes[3], XMFLOAT3 _extents)
{
    center.xmF = _center;
    memcpy(axes, _axes, 3 * sizeof(DirectX::XMFLOAT3));
    extents.xmF = _extents;
}

OBB::OBB(AABB _box)
{
    // Axis aligned
    axes[0].xmF = XMFLOAT3(1, 0, 0);
    axes[1].xmF = XMFLOAT3(0, 1, 0);
    axes[2].xmF = XMFLOAT3(0, 0, 1);

    XMVECTOR boxCenter = XMVectorDivide(XMVectorAdd(XMLoadFloat3(&_box.max), XMLoadFloat3(&_box.min)), XMVectorSet(2, 2, 2, 2));
    XMStoreFloat3(&center.xmF, boxCenter);
    XMStoreFloat3(&extents.xmF, XMVectorSubtract(XMLoadFloat3(&_box.max), XMLoadFloat3(&center.xmF)));
}

void OBB::translate(XMFLOAT3 _by) {
    XMVECTOR vecCenter = XMLoadFloat3(&center.xmF);
    XMVECTOR vecTranslation = XMLoadFloat3(&_by);

    XMStoreFloat3(&center.xmF, XMVectorAdd(vecCenter, vecTranslation));
}

void SphereBounds::translate(XMFLOAT3 _by){
    XMVECTOR vecCenter = XMLoadFloat3(&center);
    XMVECTOR vecTranslation = XMLoadFloat3(&_by);

   XMStoreFloat3(&center, XMVectorAdd(vecCenter, vecTranslation));
}

}