#include "pch.h"
#include "Transform.h"

using namespace DirectX;

static XMVECTOR NormalizeQuat(const XMFLOAT4& _quaternion) {
    return XMQuaternionNormalize(XMLoadFloat4(&_quaternion));
}


void Transform::setParent(Transform* _parent) {
    m_parent = _parent;
    markDirty();
}

Transform* Transform::getParent() const {
    return m_parent;
}

void Transform::markDirty() {
    m_localDirty = true;
    m_worldDirty = true;
}

void Transform::setLocalPosition(const XMFLOAT3& _position) {
    m_localPosition = _position;

    // Set pos in matrix
    m_localMatrix._41 = m_localPosition.x;
    m_localMatrix._42 = m_localPosition.y;
    m_localMatrix._43 = m_localPosition.z;
    markDirty();
}

void Transform::setLocalRotation(const XMFLOAT4& _rotation) {
    XMStoreFloat4(&m_localRotation, XMQuaternionNormalize(XMLoadFloat4(&_rotation)));
    markDirty();
}

void Transform::setLocalScale(const XMFLOAT3& _scale) {
    m_localScale = _scale;
    markDirty();
}

void Transform::localTranslate(const XMFLOAT3& _vector) {
    m_localPosition.x += _vector.x;
    m_localPosition.y += _vector.y;
    m_localPosition.z += _vector.z;
    markDirty();
}

void Transform::localRotate(const XMFLOAT4& _rotation) {
    XMVECTOR current = XMLoadFloat4(&m_localRotation);
    XMVECTOR delta   = NormalizeQuat(_rotation);
    
    XMVECTOR result = XMQuaternionMultiply(delta, current);

    XMStoreFloat4(&m_localRotation, XMQuaternionNormalize(result));
    markDirty();
}

void Transform::localRotate(const DirectX::XMFLOAT3& _delta) {
    XMVECTOR euler = XMQuaternionRotationRollPitchYaw(_delta.x, _delta.y, _delta.z);
    XMFLOAT4 quaternion;
    XMStoreFloat4(&quaternion, euler);
    localRotate(quaternion);
}

void Transform::localScale(const XMFLOAT3& _scale) {
    m_localScale.x *= _scale.x;
    m_localScale.y *= _scale.y;
    m_localScale.z *= _scale.z;
    markDirty();
}

void Transform::setPosition(const XMFLOAT3& _position) {
    if (m_parent) {
        XMFLOAT4X4 worldParent = m_parent->getWorldMatrix();
        XMMATRIX invParent = XMMatrixInverse(nullptr, XMLoadFloat4x4(&worldParent));

        XMVECTOR localPosition = XMVector3TransformCoord(XMLoadFloat3(&_position), invParent);
        XMStoreFloat3(&m_localPosition, localPosition);
    }
    else
        m_localPosition = _position;
    
    // Set pos in matrix
    m_localMatrix._41 = m_localPosition.x;
    m_localMatrix._42 = m_localPosition.y;
    m_localMatrix._43 = m_localPosition.z;
    markDirty();
}

void Transform::setRotation(const XMFLOAT4& _rotation) {
    XMVECTOR quaternion = NormalizeQuat(_rotation);

    if (m_parent) {
        XMFLOAT4 quaternionParent = m_parent->getRotation();
        XMVECTOR parentQuaternion = XMLoadFloat4(&quaternionParent);
        XMVECTOR localQuaternion = XMQuaternionMultiply(XMQuaternionInverse(parentQuaternion), quaternion);

        XMStoreFloat4(&m_localRotation, XMQuaternionNormalize(localQuaternion));
    }
    else
        XMStoreFloat4(&m_localRotation, quaternion);

    markDirty();
}

void Transform::setRotation(const DirectX::XMFLOAT3& _rotation) {
    XMVECTOR euler = XMQuaternionRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
    XMFLOAT4 quaternion;
    XMStoreFloat4(&quaternion, euler);
    setRotation(quaternion);
}

void Transform::setScale(const XMFLOAT3& _scale) {
    if (m_parent) {
        XMFLOAT3 parentScale = m_parent->getScale();
        m_localScale = { _scale.x / parentScale.x, _scale.y / parentScale.y, _scale.z / parentScale.z};
    }
    else
        m_localScale = _scale;

    markDirty();
}

void Transform::translate(const XMFLOAT3& _vector) {
    XMFLOAT3 worldPosition = getPosition();
    worldPosition.x += _vector.x;
    worldPosition.y += _vector.y;
    worldPosition.z += _vector.z;
    setPosition(worldPosition);
}

void Transform::rotate(const XMFLOAT4& _rotation) {
    XMFLOAT4 worldRotation = getRotation();
    XMVECTOR worldQuaternion = XMQuaternionMultiply(XMLoadFloat4(&worldRotation), NormalizeQuat(_rotation));

    XMFLOAT4 rotation;
    XMStoreFloat4(&rotation, worldQuaternion);
    setRotation(rotation);
}

void Transform::rotate(const DirectX::XMFLOAT3& _delta) {
    XMVECTOR euler = XMQuaternionRotationRollPitchYaw(_delta.x, _delta.y, _delta.z);
    XMFLOAT4 quaternion;
    XMStoreFloat4(&quaternion, euler);
    rotate(quaternion);
}

void Transform::scale(const XMFLOAT3& _scale) {
    XMFLOAT3 worldScale = getScale();
    worldScale.x *= _scale.x;
    worldScale.y *= _scale.y;
    worldScale.z *= _scale.z;
    setScale(worldScale);
}


void Transform::updateLocalMatrix() {
    XMMATRIX localMatrix = 
        XMMatrixScalingFromVector(XMLoadFloat3(&m_localScale)) *
        XMMatrixRotationQuaternion(XMLoadFloat4(&m_localRotation)) *
        XMMatrixTranslationFromVector(XMLoadFloat3(&m_localPosition));

    XMStoreFloat4x4(&m_localMatrix, localMatrix);
    m_localDirty = false;
}

void Transform::updateWorldMatrix() {
    if (m_localDirty)
        updateLocalMatrix();

    XMMATRIX local = XMLoadFloat4x4(&m_localMatrix);

    if (m_parent) {
        XMFLOAT4X4 worldParent = m_parent->getWorldMatrix();
        XMMATRIX parentWorldMatrix = XMLoadFloat4x4(&worldParent);
        XMStoreFloat4x4(&m_worldMatrix, local * parentWorldMatrix);
    }
    else
        XMStoreFloat4x4(&m_worldMatrix, local);

    XMVECTOR scale, rotation, translation;
    XMMatrixDecompose(&scale, &rotation, &translation, XMLoadFloat4x4(&m_worldMatrix));

    DirectX::XMStoreFloat4x4(&m_worldMatrix, DirectX::XMLoadFloat4x4(&m_worldMatrix));

    XMStoreFloat3(&m_worldScale, scale);
    XMStoreFloat4(&m_worldRotation, XMQuaternionNormalize(rotation));
    XMStoreFloat3(&m_worldPosition, translation);

    m_worldDirty = false;
}

XMFLOAT4X4 Transform::getLocalMatrix() {
    if (m_localDirty)
        updateLocalMatrix();

    return m_localMatrix;
}

XMFLOAT4X4 Transform::getWorldMatrix() {
    if (m_worldDirty || m_parent != nullptr)
        updateWorldMatrix();

    return m_worldMatrix;
}

XMFLOAT4X4 Transform::getTransposedWorldMatrix() {
    if (m_worldDirty || m_parent != nullptr)
        updateWorldMatrix();

    XMFLOAT4X4 transposedMatrix;
    XMStoreFloat4x4(&transposedMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
    return transposedMatrix;
}

XMFLOAT3 Transform::getLocalPosition() const { 
    return m_localPosition; 
}
XMFLOAT4 Transform::getLocalRotation() const {
    return m_localRotation; 
}
XMFLOAT3 Transform::getLocalScale() const { 
    return m_localScale; 
}

XMFLOAT3 Transform::getPosition() {
    if (m_worldDirty || m_parent != nullptr)
        updateWorldMatrix();

    return m_worldPosition;
}

XMFLOAT4 Transform::getRotation() {
    if (m_worldDirty || m_parent != nullptr)
        updateWorldMatrix();

    return m_worldRotation;
}

XMFLOAT3 Transform::getScale() {
    if (m_worldDirty || m_parent != nullptr)
        updateWorldMatrix();

    return m_worldScale;
}

XMFLOAT3 Transform::getRight() {
    XMFLOAT4 rotation = getRotation();
    XMVECTOR right = XMVector3Rotate(XMVectorSet(1, 0, 0, 0),XMLoadFloat4(&rotation));

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

XMFLOAT3 Transform::getForward() {
    XMFLOAT4 rotation = getRotation();
    XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0),XMLoadFloat4(&rotation));

    XMFLOAT3 result;
    XMStoreFloat3(&result, XMVector3Normalize(forward));
    return result;
}

void Transform::lookAt(const XMFLOAT3& _target, const XMFLOAT3& _up) {
    XMFLOAT3 worldPosition = getPosition();
    XMVECTOR direction = XMLoadFloat3(&_target) - XMLoadFloat3(&worldPosition);

    lookTo( XMFLOAT3{XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction)},_up);
}

void Transform::lookTo(const XMFLOAT3& _direction, const XMFLOAT3& _up) {
    XMMATRIX view = XMMatrixLookToLH(XMVectorZero(), XMVector3Normalize(XMLoadFloat3(&_direction)), XMLoadFloat3(&_up));

    XMMATRIX invView = XMMatrixInverse(nullptr, view);
    XMVECTOR rotation = XMQuaternionRotationMatrix(invView);

    XMFLOAT4 quaternion;
    XMStoreFloat4(&quaternion, XMQuaternionNormalize(rotation));
    setRotation(quaternion);
}
