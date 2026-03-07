#pragma once

// Local dependencies
#include "../camera.h"
#include "common/maths/transform.h"
#include "common/maths/frustum.h"

// External dependencies
#include <DirectXMath.h>

namespace angler {
    /// <summary>
    /// In degrees
    /// </summary>
    constexpr float DEFAULT_CAMERA_FOV = 65.0f;
    
    constexpr float DEFAULT_CAMERA_NEAR = 0.01f;
    constexpr float DEFAULT_CAMERA_FAR = 512.0f;
    
    class Camera3D : public Camera {
    public:
        Camera3D();
        Camera3D(float _fov, float _near = DEFAULT_CAMERA_NEAR, float _far = DEFAULT_CAMERA_FAR);
        
        float getFov() const;
        void setFov(float _fov);
        
        float getNear() const;
        void setNear(float _near);
        
        float getFar() const;
        void setFar(float _far);
        
        [[nodiscard]] DirectX::XMFLOAT4X4 getView() const;
        [[nodiscard]] DirectX::XMFLOAT4X4 getProj() const;

        [[nodiscard]] Frustum getFrustum() const;

        void setPosition(const DirectX::XMFLOAT3& _position);
        void setRotation(const DirectX::XMFLOAT4& _rotation);
        void setScale(const DirectX::XMFLOAT3& _scale);

        void translate(const DirectX::XMFLOAT3& _delta);
        void rotate(const DirectX::XMFLOAT4& _delta);
        void rotate(const DirectX::XMFLOAT3& _delta);
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

        [[nodiscard]] DirectX::XMFLOAT3 getLocalPosition() const;
        [[nodiscard]] DirectX::XMFLOAT4 getLocalRotation() const;
        [[nodiscard]] DirectX::XMFLOAT3 getLocalScale() const;

        [[nodiscard]] DirectX::XMFLOAT4X4 getWorldMatrix();

        [[nodiscard]] DirectX::XMFLOAT3 getRight();
        [[nodiscard]] DirectX::XMFLOAT3 getUp();
        [[nodiscard]] DirectX::XMFLOAT3 getForward();

        void setTransform(Transform& _transform);
        
    protected:
        Transform m_transform;

        DirectX::XMFLOAT4X4 m_view = {};
        DirectX::XMFLOAT4X4 m_proj = {};
        
        Frustum m_frustum;

        float m_fov = DEFAULT_CAMERA_FOV;
        float m_near = DEFAULT_CAMERA_NEAR;
        float m_far = DEFAULT_CAMERA_FAR;
        
    private:
        void extractFrustumPlanes();

        void update(float _aspectRatio);
        
        friend class Renderer;
    };

}