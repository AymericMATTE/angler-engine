#pragma once

#include "render/dx12/objects/sprite-sheet.h"

#include <DirectXMath.h>
#include "engine/ec/component-base.h"

namespace angler {
    struct TextComponent: public ComponentBase<TextComponent> {
        SpriteSheet* getFont() const;
        std::string getString() const;
        DirectX::XMFLOAT2 getPosition() const;
        DirectX::XMFLOAT2 getAnchor() const;
        float getRotation() const;
        float getScale() const;
        int getLayer() const;
        DirectX::XMFLOAT4 getColor() const;
        
        void setFont(SpriteSheet* _font);
        void setString(std::string _string);
        void setPosition(DirectX::XMFLOAT2 _position);
        void setAnchor(DirectX::XMFLOAT2 _anchor);
        void setRotation(float _rotation);
        void setScale(float _scale);
        void setLayer(int _layer);
        
        void setColor(DirectX::XMFLOAT4 _color);

        void OnRenderUI();

    private:
        DirectX::XMFLOAT4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f }; // Color multiplier
        SpriteSheet* m_font = nullptr;
        std::string m_string = {};
        DirectX::XMFLOAT2 m_position = {};
        DirectX::XMFLOAT2 m_anchor = {};
        float m_rotation = 0.0f;
        float m_scale = 1.0f;
        int m_layer = 0;
    };
}