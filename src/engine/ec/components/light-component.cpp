#include "pch.h"
#include "light-component.h"
#include "ec/game-object.h"
#include "application.h"

namespace angler {
    void LightComponent::OnPreRender() {
        setPosition(getOwner()->getPosition()); 
        Application::get().getRenderer().illuminate(m_light);
    }

    Type LightComponent::getType() const {
        return m_light.type;
    }
    
    float LightComponent::getIntensity() const {
        return m_light.intensity;
    }
    
    float LightComponent::getSpotInnerCos() const {
        return m_light.spotInnerCos;
    }
    
    float LightComponent::getSpotOuterCos() const {
        return m_light.spotOuterCos;
    }
    
    DirectX::XMFLOAT3 LightComponent::getColor() const {
        return m_light.color;
    }
    
    float LightComponent::getRange() const {
        return m_light.range;
    }
    
    DirectX::XMFLOAT3 LightComponent::getDirection() const {
        return m_light.direction;
    }
    
    DirectX::XMFLOAT3 LightComponent::getPosition() const {
        return m_light.position;
    }
    
    void LightComponent::setType(Type _type) {
        m_light.type = _type;
    }
    
    void LightComponent::setIntensity(float _intensity) {
        m_light.intensity = _intensity;
    }
    
    void LightComponent::setSpotInnerCos(float _spot_inner_cos) {
        m_light.spotInnerCos = _spot_inner_cos;
    }
    
    void LightComponent::setSpotOuterCos(float _spot_outer_cos) {
        m_light.spotOuterCos = _spot_outer_cos;
    }
    
    void LightComponent::setColor(DirectX::XMFLOAT3 _color) {
        m_light.color = _color;
    }
    
    void LightComponent::setRange(float _range) {
        m_light.range = _range;
    }
    
    void LightComponent::setDirection(DirectX::XMFLOAT3 _direction) {
        m_light.direction = _direction;
    }
    
    void LightComponent::setPosition(DirectX::XMFLOAT3 _position) {
        m_light.position = _position;
    }
    
    void LightComponent::directional() {
        m_light.directional();
    }
    void LightComponent::point() {
        m_light.point();
    }
}