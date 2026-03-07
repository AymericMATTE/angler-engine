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

        XMFLOAT4X4 world = m_transform.getTransposedWorldMatrix();
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

    void Camera3D::setPosition(const DirectX::XMFLOAT3& _position) {
        m_transform.setPosition(_position);
        m_dirty = true;
    }

    void Camera3D::setRotation(const DirectX::XMFLOAT4& _rotation) {
        m_transform.setRotation(_rotation);
        m_dirty = true;
    }

    void Camera3D::setScale(const DirectX::XMFLOAT3& _scale) {
        m_transform.setScale(_scale);
        m_dirty = true;
    }

    void Camera3D::translate(const DirectX::XMFLOAT3& _delta) {
        m_transform.translate(_delta);
        m_dirty = true;
    }

    void Camera3D::rotate(const DirectX::XMFLOAT4& _delta) {
        m_transform.rotate(_delta);
        m_dirty = true;
    }

    void Camera3D::rotate(const DirectX::XMFLOAT3& _rotation) {
        m_transform.rotate(_rotation);
        m_dirty = true;
    }

    void Camera3D::scale(const DirectX::XMFLOAT3& _factor) {
        m_transform.scale(_factor);
        m_dirty = true;
    }

    void Camera3D::setLocalPosition(const DirectX::XMFLOAT3& _position) {
        m_transform.setLocalPosition(_position);
        m_dirty = true;
    }

    void Camera3D::setLocalRotation(const DirectX::XMFLOAT4& _rotation) {
        m_transform.setLocalRotation(_rotation);
        m_dirty = true;
    }

    void Camera3D::setLocalScale(const DirectX::XMFLOAT3& _scale) {
        m_transform.setLocalScale(_scale);
        m_dirty = true;
    }

    void Camera3D::localTranslate(const DirectX::XMFLOAT3& _delta) {
        m_transform.localTranslate(_delta);
        m_dirty = true;
    }

    void Camera3D::localRotate(const DirectX::XMFLOAT4& _delta) {
        m_transform.localRotate(_delta);
        m_dirty = true;
    }
    void Camera3D::localRotate(const DirectX::XMFLOAT3& _delta) {
        m_transform.localRotate(_delta);
        m_dirty = true;
    }

    void Camera3D::localScale(const DirectX::XMFLOAT3& _factor) {
        m_transform.localScale(_factor);
        m_dirty = true;
    }

    void Camera3D::lookAt(const DirectX::XMFLOAT3& _target, const DirectX::XMFLOAT3& up) {
        m_transform.lookAt(_target, up);
        m_dirty = true;
    }

    void Camera3D::lookTo(const DirectX::XMFLOAT3& _direction, const DirectX::XMFLOAT3& up) {
        m_transform.lookTo(_direction, up);
        m_dirty = true;
    }

    DirectX::XMFLOAT3 Camera3D::getPosition() {
        return m_transform.getPosition();
    }

    DirectX::XMFLOAT4 Camera3D::getRotation() {
        return m_transform.getRotation();
    }

    DirectX::XMFLOAT3 Camera3D::getScale() {
        return m_transform.getScale();
    }

    DirectX::XMFLOAT3 Camera3D::getLocalPosition() const {
        return m_transform.getLocalPosition();
    }

    DirectX::XMFLOAT4 Camera3D::getLocalRotation() const {
        return m_transform.getLocalRotation();
    }

    DirectX::XMFLOAT3 Camera3D::getLocalScale() const {
        return m_transform.getLocalScale();
    }

    DirectX::XMFLOAT4X4 Camera3D::getWorldMatrix() {
        return m_transform.getWorldMatrix();
    }

    DirectX::XMFLOAT3 Camera3D::getRight() {
        return m_transform.getRight();
    }

    DirectX::XMFLOAT3 Camera3D::getUp() {
        return m_transform.getUp();
    }

    DirectX::XMFLOAT3 Camera3D::getForward() {
        return m_transform.getForward();
    }

    void Camera3D::setTransform(Transform& _transform)
    {
        m_transform = _transform;
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