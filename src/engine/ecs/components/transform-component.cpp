#include "pch.h"
#include "transform-component.h"

#include <application.h>

namespace angler {
    void TransformComponent::setParent(TransformComponent* _parent) {
        if (_parent == nullptr) {
            m_parent = ECS::invalidEntity;
            m_transform.setParent(nullptr);
            m_tick++;
            return;
        }
        
        m_parent = _parent->entity;
        m_transform.setParent(&_parent->m_transform);
        m_tick++;
    }
    
    TransformComponent* TransformComponent::getParent() const {
        if (m_parent == ECS::invalidEntity) {
            return nullptr;
        }
        TransformComponent& transform = APPLICATION.getECS().getComponent<TransformComponent>(m_parent);
        return &transform;
    }
    
    void TransformComponent::setPosition(const DirectX::XMFLOAT3& _position) {
        updateParent();
        m_transform.setPosition(_position);
        m_tick++;
    }
    
    void TransformComponent::setRotation(const DirectX::XMFLOAT4& _rotation) {
        updateParent();
        m_transform.setRotation(_rotation);
        m_tick++;
    }
    void TransformComponent::setRotation(const DirectX::XMFLOAT3& _rotation) {
        updateParent();
        m_transform.setRotation(_rotation);
    }

    void TransformComponent::setScale(const DirectX::XMFLOAT3& _scale) {
        updateParent();
        m_transform.setScale(_scale);
        m_tick++;
    }
    
    void TransformComponent::translate(const DirectX::XMFLOAT3& _delta) {
        updateParent();
        m_transform.translate(_delta);
        m_tick++;
    }
    
    void TransformComponent::rotate(const DirectX::XMFLOAT4& _delta) {
        updateParent();
        m_transform.rotate(_delta);
        m_tick++;
    }
    
    void TransformComponent::rotate(const DirectX::XMFLOAT3& _rotation) {
        updateParent();
        m_transform.rotate(_rotation);
        m_tick++;
    }

    void TransformComponent::scale(const DirectX::XMFLOAT3& _factor) {
        updateParent();
        m_transform.scale(_factor);
        m_tick++;
    }
    
    void TransformComponent::setLocalPosition(const DirectX::XMFLOAT3& _position) {
        updateParent();
        m_transform.setLocalPosition(_position);
        m_tick++;
    }
    
    void TransformComponent::setLocalRotation(const DirectX::XMFLOAT4& _rotation) {
        updateParent();
        m_transform.setLocalRotation(_rotation);
        m_tick++;
    }
    
    void TransformComponent::setLocalScale(const DirectX::XMFLOAT3& _scale) {
        updateParent();
        m_transform.setLocalScale(_scale);
        m_tick++;
    }
    
    void TransformComponent::localTranslate(const DirectX::XMFLOAT3& _delta) {
        updateParent();
        m_transform.localTranslate(_delta);
        m_tick++;
    }
    
    void TransformComponent::localRotate(const DirectX::XMFLOAT4& _delta) {
        updateParent();
        m_transform.localRotate(_delta);
        m_tick++;
    }
    void TransformComponent::localRotate(const DirectX::XMFLOAT3& _delta) {
        updateParent();
        m_transform.localRotate(_delta);
        m_tick++;
    }

    void TransformComponent::localScale(const DirectX::XMFLOAT3& _factor) {
        updateParent();
        m_transform.localScale(_factor);
        m_tick++;
    }
    
    void TransformComponent::lookAt(const DirectX::XMFLOAT3& _target, const DirectX::XMFLOAT3& up) {
        updateParent();
        m_transform.lookAt(_target, up);
        m_tick++;
    }
    
    void TransformComponent::lookTo(const DirectX::XMFLOAT3& _direction, const DirectX::XMFLOAT3& up) {
        updateParent();
        m_transform.lookTo(_direction, up);
        m_tick++;
    }
    
    DirectX::XMFLOAT3 TransformComponent::getPosition() {
        updateParent();
        return m_transform.getPosition();
    }
    
    DirectX::XMFLOAT4 TransformComponent::getRotation() {
        updateParent();
        return m_transform.getRotation();
    }
    
    DirectX::XMFLOAT3 TransformComponent::getScale() {
        updateParent();
        return m_transform.getScale();
    }
    
    DirectX::XMFLOAT3 TransformComponent::getLocalPosition() {
        updateParent();
        return m_transform.getLocalPosition();
    }
    
    DirectX::XMFLOAT4 TransformComponent::getLocalRotation() {
        updateParent();
        return m_transform.getLocalRotation();
    }
    
    DirectX::XMFLOAT3 TransformComponent::getLocalScale() {
        updateParent();
        return m_transform.getLocalScale();
    }
    
    DirectX::XMFLOAT4X4 TransformComponent::getLocalMatrix() {
        updateParent();
        return m_transform.getLocalMatrix();
    }
    
    DirectX::XMFLOAT4X4 TransformComponent::getWorldMatrix() {
        updateParent();
        return m_transform.getWorldMatrix();
    }
    
    DirectX::XMFLOAT3 TransformComponent::getRight() {
        updateParent();
        return m_transform.getRight();
    }
    
    DirectX::XMFLOAT3 TransformComponent::getUp() {
        updateParent();
        return m_transform.getUp();
    }
    
    DirectX::XMFLOAT3 TransformComponent::getForward() {
        updateParent();
        return m_transform.getForward();
    }

    void TransformComponent::updateParent() {
        if (getParent() != nullptr) {
            m_transform.setParent(&getParent()->m_transform);
        }
    }
}