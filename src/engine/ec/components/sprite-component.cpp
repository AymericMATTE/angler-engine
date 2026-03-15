#include "pch.h"
#include "sprite-component.h"
#include "application.h"

namespace angler {
    SpriteSheet* SpriteComponent::getSprite() const {
        return m_sprite;
    }
    DirectX::XMFLOAT2 SpriteComponent::getPosition() const {
        return m_position;
    }

    DirectX::XMFLOAT2 SpriteComponent::getAnchor() const {
        return m_anchor;
    }

    float SpriteComponent::getRotation() const {
        return m_rotation;
    }

    DirectX::XMFLOAT2 SpriteComponent::getScale() const {
        return m_scale;
    }

    int SpriteComponent::getLayer() const {
        return m_layer;
    }
    
    DirectX::XMFLOAT4 SpriteComponent::getColor() const {
        return m_color;
    }
    
    void SpriteComponent::setColor(DirectX::XMFLOAT4 _color) {
        m_color = _color;
    }

    void SpriteComponent::setSprite(SpriteSheet* _sprite) {
        m_sprite = _sprite;
    }

    void SpriteComponent::setPosition(DirectX::XMFLOAT2 _position) {
        m_position = _position;
    }

    void SpriteComponent::setAnchor(DirectX::XMFLOAT2 _anchor) {
        m_anchor = _anchor;
    }

    void SpriteComponent::setRotation(float _rotation) {
        m_rotation = _rotation;
    }

    void SpriteComponent::setScale(DirectX::XMFLOAT2 _scale) {
        m_scale = _scale;
    }

    void SpriteComponent::setLayer(int _layer) {
        m_layer = _layer;
    }

    void SpriteComponent::OnRenderUI() {
        Application::get().getRenderer().drawSprite(getSprite(), getPosition(), getLayer(), getAnchor(), getScale(), getRotation(), getColor());
    }
}