#pragma once

#include <DirectXMath.h>
#include "common/types.h"
#include "common/physics.h"
#include "engine/ecs/component-base.h"

namespace angler {
    struct ColliderComponent: public ComponentBase {
        ColliderComponent() = default;

        void SetBox(DirectX::XMFLOAT3 _halfExtent = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3 _offsetFromTransform = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
        void SetSphere(float _radius = 0.5f, DirectX::XMFLOAT3 _offsetFromTransform = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

        ColliderComponent* getColliding();

    private:
        GameBounds* m_bounds = nullptr;
        DirectX::XMFLOAT3 m_transformOffset = {};
        uint m_tick = 0;

        float m_baseRadius = 0.0f;
        DirectX::XMFLOAT3 m_baseExtents = {};

        ColliderComponent* m_colliding = nullptr;

        // HGrid data
        ColliderComponent* next = nullptr;
        int bucket = 0;
        uint level = 0;

        void UpdateBounds();

        friend class PhysicsManager;
        friend class HGrid;
        friend class Application;
    }; 
}