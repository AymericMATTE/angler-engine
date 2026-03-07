#pragma once
#include <DirectXMath.h>

#include "common/maths/frustum.h"
#include "common/maths/transform.h"

namespace angler {

#define EPSILON 0.0001f

    union Vector3 {
        DirectX::XMFLOAT3 xmF;
        float arr[3];
    };


    struct AABB  {
        DirectX::XMFLOAT3 min = { -0.5f, -0.5f, -0.5f };
        DirectX::XMFLOAT3 max = { 0.5f, 0.5f, 0.5f };

        AABB() = default;
        AABB(const DirectX::XMFLOAT3& _center, const DirectX::XMFLOAT3& _halfExtents);

        void translate(DirectX::XMFLOAT3 _by);
    };

    struct OBB  {
        Vector3 center;
        Vector3 axes[3];
        Vector3 extents;

        OBB(DirectX::XMFLOAT3 _center, DirectX::XMFLOAT3 _axes[3], DirectX::XMFLOAT3 _extents);
        OBB(AABB _box);

        void translate(DirectX::XMFLOAT3 _by);
        //void rotate(float yaw, float pitch, float roll);
    };

    struct SphereBounds  {
        DirectX::XMFLOAT3 center;
        float radius = 1.0f;

        void translate(DirectX::XMFLOAT3 _by);
    };

    struct GameBounds {
    public:
        enum BoundsType {
            SPHERE = 0,
            BOX = 1
        };

        OBB* box = nullptr;
        SphereBounds* sphere = nullptr;

        BoundsType type = BOX;

        GameBounds(const OBB& _box);
        GameBounds(OBB* _box);
        GameBounds(const SphereBounds& _sphere);
        GameBounds(SphereBounds* _sphere);
        ~GameBounds();

        //void translate(DirectX::XMFLOAT3 _by);
    };

    class Physics {
    public:
        static DirectX::XMFLOAT3 closestFromAABB(DirectX::XMFLOAT3 _point, AABB _aabb);
        static DirectX::XMFLOAT3 ClosestFromOBB(DirectX::XMFLOAT3 _point, OBB _aabb);
        static float sqDistFromAABB(DirectX::XMFLOAT3 _point, AABB _aabb);
        static SphereBounds sphereFromAABB(AABB* _aabb);
        static SphereBounds sphereFromOBB(OBB* _obb);

        static bool isOnFrustum(AABB& _aabb, const Frustum& frustum, Transform& transform);
        static bool isOnFrustum(SphereBounds& _sphere, const Frustum& frustum, Transform& transform);

        static bool isOnPlane(const AABB& _aabb, const Plane& _plane);
        static bool isOnPlane(const SphereBounds& _sphere, const Plane& _plane);

        static bool intersects(SphereBounds& a, SphereBounds& b);
        static bool intersects(AABB& a, SphereBounds& b);
        static bool intersects(SphereBounds& a, AABB& b);
        static bool intersects(AABB& a, AABB& b);
        static bool intersects(OBB& a, OBB& b);
        static bool intersects(OBB& a, SphereBounds& b);
        static bool intersects(SphereBounds& a, OBB& b);
    };
}
