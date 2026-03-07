#include "pch.h"
#include "light.h"

namespace angler {
    void Light::directional() {
        type = LIGHT_TYPE_DIRECTIONAL;
        color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
        direction = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);
        intensity = 1.0f;
        
        range = 0.0f;
        position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        spotInnerCos = 0.0f;
        spotOuterCos = 0.0f;
    }
    void Light::point() {
        type = LIGHT_TYPE_POINT;
        color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
        position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        range = 1.0f;
        intensity = 5.0f;
        
        direction = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        spotInnerCos = 0.0f;
        spotOuterCos = 0.0f;
    }
}