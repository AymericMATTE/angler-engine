#include "pch.h"
#include "camera-3d.h"

namespace angler {

    using namespace DirectX;

    XMFLOAT4X4 Camera3D::getView() const {
        return m_view;
    }
    XMFLOAT4X4 Camera3D::getProj() const {
        return m_proj;
    }

    Frustum Camera3D::getFrustum() const {
        return m_frustum;
    }

    Camera3D::Camera3D() {
        Camera3D(65.0f);
    }

    Camera3D::Camera3D(float _fov, float _near, float _far) {
        m_fov = _fov;
        m_near = _near;
        m_far = _far;
        m_dirty = true;
    }
    
    float Camera3D::getFov() const {
        return m_fov;
    }
    
    void Camera3D::setFov(float _fov) {
        m_fov = _fov;
        m_dirty = true;
    }
    
    float Camera3D::getNear() const {
        return m_near;
    }
    
    void Camera3D::setNear(float _near) {
        m_near = _near;
        m_dirty = true;
    }
    
    float Camera3D::getFar() const {
        return m_far;
    }
    
    void Camera3D::setFar(float _far) {
        m_far = _far;
        m_dirty = true;
    }

    void Camera3D::update(float _aspectRatio) {
        if (!m_dirty)
            return;
        
        //////////////// VIEW ////////////////

        XMFLOAT4X4 world;
        XMStoreFloat4x4(&world, XMMatrixTranspose(XMLoadFloat4x4(&m_world)));
        XMMATRIX view = XMMatrixInverse(nullptr, XMLoadFloat4x4(&world));
        XMStoreFloat4x4(&m_view, view);

        /////////////// PROJ /////////////////
        
        float radFOV = XMConvertToRadians(m_fov);
        
        XMMATRIX proj = XMMatrixTranspose(
            XMMatrixPerspectiveFovLH(radFOV, _aspectRatio, m_near, m_far)
        );
        XMStoreFloat4x4(&m_proj, proj);

        ////////////// FRUSTUM ///////////////
        extractFrustumPlanes();

        m_dirty = false;
    }

    DirectX::XMFLOAT3 Camera3D::getPosition() {
        return XMFLOAT3(m_world._41, m_world._42, m_world._43);
    }

    DirectX::XMFLOAT4X4 Camera3D::getWorldMatrix() {
        return m_world;
    }

    void Camera3D::setWorldMatrix(const XMFLOAT4X4& _world) {
        memcpy(&m_world, &_world, sizeof(XMFLOAT4X4));
        m_dirty = true;
    }
    
    void Camera3D::extractFrustumPlanes()
    {
        XMMATRIX M = XMMatrixMultiply(XMMatrixTranspose(XMLoadFloat4x4(&m_view)),XMMatrixTranspose(XMLoadFloat4x4(&m_proj)));
        XMFLOAT4X4 viewProj;
        XMStoreFloat4x4(&viewProj,M);
        
        XMVECTOR left   = XMVectorSet(viewProj._14 + viewProj._11, viewProj._24 + viewProj._21, viewProj._34 + viewProj._31, viewProj._44 + viewProj._41);
        XMVECTOR right  = XMVectorSet(viewProj._14 - viewProj._11, viewProj._24 - viewProj._21, viewProj._34 - viewProj._31, viewProj._44 - viewProj._41);
        XMVECTOR bottom = XMVectorSet(viewProj._14 + viewProj._12, viewProj._24 + viewProj._22, viewProj._34 + viewProj._32, viewProj._44 + viewProj._42);
        XMVECTOR top    = XMVectorSet(viewProj._14 - viewProj._12, viewProj._24 - viewProj._22, viewProj._34 - viewProj._32, viewProj._44 - viewProj._42);
        
        XMVECTOR nearP  = XMVectorSet(viewProj._13, viewProj._23, viewProj._33, viewProj._43);
        XMVECTOR farP   = XMVectorSet(viewProj._14-viewProj._13, viewProj._24-viewProj._23, viewProj._34-viewProj._33, viewProj._44-viewProj._43);
        
        m_frustum.leftFace.CreatePlaneFromVector(left);
        m_frustum.rightFace.CreatePlaneFromVector(right);
        m_frustum.bottomFace.CreatePlaneFromVector(bottom);
        m_frustum.topFace.CreatePlaneFromVector(top);
        m_frustum.nearFace.CreatePlaneFromVector(nearP);
        m_frustum.farFace.CreatePlaneFromVector(farP);
    }
}