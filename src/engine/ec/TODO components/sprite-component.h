#pragma once

#include "render/dx12/objects/sprite-sheet.h"

#include <DirectXMath.h>
#include "engine/ecs/component-base.h"

namespace angler {
    struct SpriteComponent: public ComponentBase {
        SpriteSheet* getSprite() const;
        DirectX::XMFLOAT2 getPosition() const;
        DirectX::XMFLOAT2 getAnchor() const;
        float getRotation() const;
        DirectX::XMFLOAT2 getScale() const;
        int getLayer() const;
        DirectX::XMFLOAT4 getColor() const;

        void setSprite(SpriteSheet* _sprite);
        void setPosition(DirectX::XMFLOAT2 _position);
        void setAnchor(DirectX::XMFLOAT2 _anchor);
        void setRotation(float _rotation);
        void setScale(DirectX::XMFLOAT2 _scale);
        void setLayer(int _layer);
        
        void setColor(DirectX::XMFLOAT4 _color);

    private:
        DirectX::XMFLOAT4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f }; // Color multiplier
        SpriteSheet* m_sprite = nullptr;
        DirectX::XMFLOAT2 m_position = {};
        int m_layer = 0;
        DirectX::XMFLOAT2 m_anchor = {};
        float m_rotation = 0.0f;
        DirectX::XMFLOAT2 m_scale = { 1.0f, 1.0f };
    };
}