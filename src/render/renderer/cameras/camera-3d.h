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

        [[nodiscard]] DirectX::XMFLOAT3 getPosition();
        [[nodiscard]] DirectX::XMFLOAT4X4 getWorldMatrix();
        
        void setWorldMatrix(const DirectX::XMFLOAT4X4& _world);

    protected:
        DirectX::XMFLOAT4X4 m_world = {};
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