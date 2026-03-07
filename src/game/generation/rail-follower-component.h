#pragma once

#include "ecs/component-base.h"
#include <DirectXMath.h>

struct RailFollower : public angler::ComponentBase {
    float speed = 3.0f;
    float t = 0.0f; // progress
    angler::ECS::Entity currentChunk = angler::ECS::invalidEntity;
    bool active = true;
    DirectX::XMFLOAT3 offsetPosition = DirectX::XMFLOAT3( 0, 0, 0 );
};
