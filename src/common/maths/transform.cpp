#include "pch.h"
#include "Transform.h"

using namespace DirectX;

XMFLOAT4 eulerToQuaternion(const XMFLOAT3& _euler) {
    XMVECTOR localDeltaEuler = XMLoadFloat3(&_euler);
    XMVECTOR vect = XMQuaternionRotationRollPitchYawFromVector(localDeltaEuler);
    XMFLOAT4 quat;
    XMStoreFloat4(&quat, vect);
    return quat;
}

Transform::Transform() {
    XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

void Transform::setDirty() {
    if (m_isDirty) 
        return;

    m_isDirty = true;
}

void Transform::setParent(Transform* _parent) {
    if (m_parent == _parent) 
        return;

    m_parent = _parent;

    setDirty();
}

void Transform::setPosition(const XMFLOAT3& _pos) {
    if (m_parent) {
        XMFLOAT4X4 parentWorld = m_parent->getWorldMatrix();
        XMMATRIX invParentWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&parentWorld));

        XMVECTOR localPos = XMVector3TransformCoord(XMLoadFloat3(&_pos), invParentWorld);
        XMStoreFloat3(&m_localPosition, localPos);
    }
    else {
        m_localPosition = _pos;
    }
    setDirty();
}

void Transform::setRotation(const XMFLOAT4& _quat) {
    XMVECTOR worldQuat = XMQuaternionNormalize(XMLoadFloat4(&_quat));

    if (m_parent) {
        XMFLOAT4 parentWorldRot = m_parent->getRotation();
        XMVECTOR localQuat = XMQuaternionMultiply(worldQuat, XMQuaternionInverse(XMLoadFloat4(&parentWorldRot)));

        XMStoreFloat4(&m_localRotation, XMQuaternionNormalize(localQuat));
    }
    else {
        XMStoreFloat4(&m_localRotation, worldQuat);
    }
    setDirty();
}

void Transform::setRotation(const XMFLOAT3& _euler) {
    setRotation(eulerToQuaternion(_euler));
}

void Transform::setScale(const XMFLOAT3& _scale) {
    if (m_parent) {
        XMFLOAT3 parentScale = m_parent->getScale();
        XMVECTOR localScale = XMVectorDivide(XMLoadFloat3(&_scale), XMLoadFloat3(&parentScale));

        XMStoreFloat3(&m_localScale, localScale);
    }
    else {
        m_localScale = _scale;
    }
    setDirty();
}

void Transform::localTranslate(const XMFLOAT3& _delta) {
    XMVECTOR localDelta = XMLoadFloat3(&_delta);
    XMVECTOR currentPos = XMLoadFloat3(&m_localPosition);
    XMStoreFloat3(&m_localPosition, XMVectorAdd(currentPos, localDelta));
    setDirty();
}

void Transform::localRotate(const XMFLOAT4& _deltaQuat) {
    XMVECTOR localDeltaQuat = XMLoadFloat4(&_deltaQuat);
    XMVECTOR currentRot = XMLoadFloat4(&m_localRotation);
    XMStoreFloat4(&m_localRotation, XMQuaternionNormalize(XMQuaternionMultiply(localDeltaQuat, currentRot)));
    setDirty();
}

void Transform::localRotate(const XMFLOAT3& _deltaEuler) {
    localRotate(eulerToQuaternion(_deltaEuler));
}

void Transform::localScale(const XMFLOAT3& factor) {    
    XMVECTOR localFactor = XMLoadFloat3(&factor);
    XMVECTOR currentSca = XMLoadFloat3(&m_localScale);
    XMStoreFloat4(&m_localRotation, XMVectorMultiply(localFactor, currentSca));
    setDirty();
}

void Transform::setWorldTransform(const XMFLOAT3& _pos, const XMFLOAT4& _quat, const XMFLOAT3& _scale) {
    if (m_parent) {
        XMFLOAT3 parentSca = m_parent->getScale();
        XMFLOAT4X4 parentWorld = m_parent->getWorldMatrix();
        XMFLOAT4 parentRot = m_parent->getRotation();

        XMMATRIX invParentMat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&parentWorld));
        XMVECTOR invParentRot = XMQuaternionInverse(XMLoadFloat4(&parentRot));

        XMVECTOR lPos = XMVector3TransformCoord(XMLoadFloat3(&_pos), invParentMat);
        XMVECTOR lRot = XMQuaternionMultiply(XMQuaternionNormalize(XMLoadFloat4(&_quat)), invParentRot);
        XMVECTOR lSca = XMVectorDivide(XMLoadFloat3(&_scale), XMLoadFloat3(&parentSca));

        XMStoreFloat3(&m_localPosition, lPos);
        XMStoreFloat4(&m_localRotation, XMQuaternionNormalize(lRot));
        XMStoreFloat3(&m_localScale, lSca);
    }
    else {
        m_localPosition = _pos;
        m_localRotation = _quat;
        m_localScale = _scale;
    }

    setDirty();
}

void Transform::translate(const XMFLOAT3& _delta) {
    updateValues();
    XMVECTOR newWorldPos = XMVectorAdd(XMLoadFloat3(&m_worldPosition), XMLoadFloat3(&_delta));
    XMFLOAT3 res;
    XMStoreFloat3(&res, newWorldPos);
    setPosition(res);
}

void Transform::rotate(const XMFLOAT4& _deltaQuat) {
    updateValues();
    XMVECTOR currentWorldRot = XMLoadFloat4(&m_worldRotation);
    XMVECTOR delta = XMLoadFloat4(&_deltaQuat);
    XMVECTOR newRot = XMQuaternionMultiply(delta, currentWorldRot);

    XMFLOAT4 res;
    XMStoreFloat4(&res, XMQuaternionNormalize(newRot));
    setRotation(res);
}

void Transform::rotate(const XMFLOAT3& _deltaEuler) {
    rotate(eulerToQuaternion(_deltaEuler));
}

void Transform::scale(const XMFLOAT3& _factor) {
    updateValues();
    XMVECTOR newWorldScale = XMVectorMultiply(XMLoadFloat3(&m_worldScale), XMLoadFloat3(&_factor));
    XMFLOAT3 res;
    XMStoreFloat3(&res, newWorldScale);
    setScale(res);
}

void Transform::setLocalPosition(const XMFLOAT3& _pos) {
    m_localPosition = _pos; 
    setDirty();
}

void Transform::setLocalRotation(const XMFLOAT4& _quat) { 
    XMStoreFloat4(&m_localRotation, XMQuaternionNormalize(XMLoadFloat4(&_quat)));
    setDirty();
}

void Transform::setLocalRotation(const XMFLOAT3& _euler) {
    setLocalRotation(eulerToQuaternion(_euler));
}

void Transform::setLocalScale(const XMFLOAT3& _scale) { 
    m_localScale = _scale; 
    setDirty(); 
}

void Transform::lookAt(const XMFLOAT3& _target, const XMFLOAT3& _up) {
    XMFLOAT3 worldPosition = getPosition();
    XMVECTOR direction = XMLoadFloat3(&_target) - XMLoadFloat3(&worldPosition);
    lookTo(XMFLOAT3{ XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction) }, _up);
}

void Transform::lookTo(const XMFLOAT3& _direction, const XMFLOAT3& _up) {
    XMMATRIX view = XMMatrixLookToLH(XMVectorZero(), XMVector3Normalize(XMLoadFloat3(&_direction)), XMLoadFloat3(&_up));
    XMMATRIX invView = XMMatrixInverse(nullptr, view);
    XMVECTOR rotation = XMQuaternionRotationMatrix(invView);
    XMFLOAT4 quaternion;
    XMStoreFloat4(&quaternion, XMQuaternionNormalize(rotation));
    setRotation(quaternion);
}

void Transform::updateValues() {
    if (!m_isDirty && m_parent == nullptr)
        return;

    XMVECTOR lPos = XMLoadFloat3(&m_localPosition);
    XMVECTOR lRot = XMLoadFloat4(&m_localRotation);
    XMVECTOR lSca = XMLoadFloat3(&m_localScale);

    XMMATRIX localMat = XMMatrixScalingFromVector(lSca) *
        XMMatrixRotationQuaternion(lRot) *
        XMMatrixTranslationFromVector(lPos);

    if (m_parent) {
        XMFLOAT4X4 parentMat = m_parent->getWorldMatrix();
        XMMATRIX worldMat = localMat * XMLoadFloat4x4(&parentMat);
        XMStoreFloat4x4(&m_worldMatrix, worldMat);

        XMStoreFloat3(&m_worldPosition, worldMat.r[3]);

        XMFLOAT4 parentRot = m_parent->getRotation();
        XMStoreFloat4(&m_worldRotation, XMQuaternionMultiply(lRot, XMLoadFloat4(&parentRot)));

        XMFLOAT3 parentSca = m_parent->getScale();
        XMStoreFloat3(&m_worldScale, XMVectorMultiply(lSca, XMLoadFloat3(&parentSca)));
    }
    else {
        XMStoreFloat4x4(&m_worldMatrix, localMat);
        m_worldPosition = m_localPosition;
        m_worldRotation = m_localRotation;
        m_worldScale = m_localScale;
    }

    m_isDirty = false;
}

XMFLOAT4X4 Transform::getWorldMatrix() {
    updateValues(); 
    return m_worldMatrix; 
}

XMFLOAT4X4 Transform::getTransposedWorldMatrix() {
    updateValues();
    
    XMMATRIX matTransWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix));
    XMFLOAT4X4 transposedWorld;
    XMStoreFloat4x4(&transposedWorld, matTransWorld);
    return transposedWorld;
}

XMFLOAT3 Transform::getPosition() { 
    updateValues(); 
    return m_worldPosition; 
}

XMFLOAT4 Transform::getRotation() { 
    updateValues(); 
    return m_worldRotation; 
}

XMFLOAT3 Transform::getScale() { 
    updateValues(); 
    return m_worldScale; 
}

XMFLOAT3 Transform::getForward() {
    XMFLOAT4 rotation = getRotation();
    XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&rotation));

    XMFLOAT3 result;
    XMStoreFloat3(&result, XMVector3Normalize(forward));
    return result;
}

XMFLOAT3 Transform::getRight() {
    XMFLOAT4 rotation = getRotation();
    XMVECTOR right = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), XMLoadFloat4(&rotation));

    XMFLOAT3 result;
    XMStoreFloat3(&result, XMVector3Normalize(right));
    return result;
}

XMFLOAT3 Transform::getUp() {
    XMFLOAT4 rotation = getRotation();
    XMVECTOR up = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), XMLoadFloat4(&rotation));

    XMFLOAT3 result;
    XMStoreFloat3(&result, XMVector3Normalize(up));
    return result;
}