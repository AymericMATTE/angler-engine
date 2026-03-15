#pragma once
#include "render/renderer/light.h"
#include "engine/ec/component-base.h"

namespace angler {
    struct LightComponent : public ComponentBase<LightComponent> {
    public:
        void OnPreRender();

        Type getType() const;
        float getIntensity() const;
        float getSpotInnerCos() const;
        float getSpotOuterCos() const;
        DirectX::XMFLOAT3 getColor() const;
        float getRange() const;
        DirectX::XMFLOAT3 getDirection() const;
        DirectX::XMFLOAT3 getPosition() const;

        void setType(Type _type);
        void setIntensity(float _intensity);
        void setSpotInnerCos(float _spot_inner_cos);
        void setSpotOuterCos(float _spot_outer_cos);
        void setColor(DirectX::XMFLOAT3 _color);
        void setRange(float _range);
        void setDirection(DirectX::XMFLOAT3 _direction);
        void setPosition(DirectX::XMFLOAT3 _position);

        void directional();
        void point();

    private:
        Light m_light = {};
        friend class Application;
    };
}