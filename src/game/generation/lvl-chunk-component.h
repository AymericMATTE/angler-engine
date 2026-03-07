#pragma once

#include "ecs/component-base.h"
#include <DirectXMath.h>

namespace angler {
    struct ChunkComponent : public ComponentBase {
        ECS::Entity exitEntity = ECS::invalidEntity;
        ECS::Entity waterEntity = ECS::invalidEntity;
        
        std::vector<ECS::Entity> enemies = { ECS::invalidEntity };
        std::vector<ECS::Entity> crates = { ECS::invalidEntity };
        
        // Chunk chain
        ECS::Entity previous = ECS::invalidEntity;
        ECS::Entity next = ECS::invalidEntity;
        
        uint generation = 0;
        byte typeIndex = 0;
        
        // Position for next chunks
        DirectX::XMFLOAT3 worldPos;
        DirectX::XMFLOAT4 worldRot;

        // Chunk deletion
        bool markedForDeletion = false;
    };
}