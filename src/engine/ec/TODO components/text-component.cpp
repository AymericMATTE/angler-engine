#include "pch.h"
#include "text-component.h"

namespace angler {
    SpriteSheet* TextComponent::getFont() const {
        return m_font;
    }
    
    std::string TextComponent::getString() const {
        return m_string;
    }
    
    DirectX::XMFLOAT2 TextComponent::getPosition() const {
        return m_position;
    }
    
    DirectX::XMFLOAT2 TextComponent::getAnchor() const {
        return m_anchor;
    }
    
    float TextComponent::getRotation() const {
        return m_rotation;
    }

    float TextComponent::getScale() const {
        return m_scale;
    }

    int TextComponent::getLayer() const {
        return m_layer;
    }
    DirectX::XMFLOAT4 TextComponent::getColor() const {
        return m_color;
    }
    
    void TextComponent::setColor(DirectX::XMFLOAT4 _color) {
        m_color = _color;
    }

    void TextComponent::setFont(SpriteSheet* _font) {
        m_font = _font;
        m_font->Cut(126 - 32, 10);
    }
    
    void TextComponent::setString(std::string _string) {
        m_string = _string;
    }
    
    void TextComponent::setPosition(DirectX::XMFLOAT2 _position) {
        m_position = _position;
    }
    
    void TextComponent::setAnchor(DirectX::XMFLOAT2 _anchor) {
        m_anchor = _anchor;
    }
    
    void TextComponent::setRotation(float _rotation) {
        m_rotation = _rotation;
    }

    void TextComponent::setScale(float _scale) {
        m_scale = _scale;
    }

    void TextComponent::setLayer(int _layer) {
        m_layer = _layer;
    }

}