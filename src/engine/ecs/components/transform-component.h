#pragma once

#include <DirectXMath.h>

#include "common/maths/transform.h"
#include "common/types.h"
#include "engine/ecs/component-base.h"

namespace angler {
    struct TransformComponent: public ComponentBase {
        void setParent(TransformComponent* _parent);
        [[nodiscard]] TransformComponent* getParent() const;

        void setPosition(const DirectX::XMFLOAT3& _position);
        void setRotation(const DirectX::XMFLOAT4& _rotation);
        void setRotation(const DirectX::XMFLOAT3& _rotation);
        void setScale(const DirectX::XMFLOAT3& _scale);

        void translate(const DirectX::XMFLOAT3& _delta);
        void rotate(const DirectX::XMFLOAT4& _delta);
        void rotate(const DirectX::XMFLOAT3& _rotation);
        void scale(const DirectX::XMFLOAT3& _factor);

        void setLocalPosition(const DirectX::XMFLOAT3& _position);
        void setLocalRotation(const DirectX::XMFLOAT4& _rotation);
        void setLocalScale(const DirectX::XMFLOAT3& _scale);

        void localTranslate(const DirectX::XMFLOAT3& _delta);
        void localRotate(const DirectX::XMFLOAT4& _delta);
        void localRotate(const DirectX::XMFLOAT3& _delta);
        void localScale(const DirectX::XMFLOAT3& _factor);

        void lookAt(const DirectX::XMFLOAT3& _target, const DirectX::XMFLOAT3& up = { 0.f, 1.f, 0.f });
        void lookTo(const DirectX::XMFLOAT3& _direction, const DirectX::XMFLOAT3& up = { 0.f, 1.f, 0.f });

        [[nodiscard]] DirectX::XMFLOAT3 getPosition();
        [[nodiscard]] DirectX::XMFLOAT4 getRotation();
        [[nodiscard]] DirectX::XMFLOAT3 getScale();

        [[nodiscard]] DirectX::XMFLOAT3 getLocalPosition();
        [[nodiscard]] DirectX::XMFLOAT4 getLocalRotation();
        [[nodiscard]] DirectX::XMFLOAT3 getLocalScale();

        [[nodiscard]] DirectX::XMFLOAT4X4 getWorldMatrix();

        [[nodiscard]] DirectX::XMFLOAT3 getRight();
        [[nodiscard]] DirectX::XMFLOAT3 getUp();
        [[nodiscard]] DirectX::XMFLOAT3 getForward();
        
    protected:
        void updateParent();

        uint m_tick = 0;
        Transform m_transform = {};
        ECS::Entity m_parent = ECS::invalidEntity;
        friend class Application;
    }; 
}