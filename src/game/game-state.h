#pragma once

// External dependencies
#include "ecs/ecs.h"
#include "generation/lvl-chunk-component.h"

// Forward declarations
namespace angler {
    struct TextComponent;
    struct SpriteComponent;
    struct ChunkComponent;
}

namespace angler {
    struct GameState {
        // Player related members
        ECS::Entity player = ECS::invalidEntity;
        ECS::Entity vehicle = ECS::invalidEntity;
        uint currentChunk = 0;

        // UI related members
        TextComponent* scoreText = nullptr;
        SpriteComponent* healthBar = nullptr;
        SpriteComponent* healthBarOutline = nullptr;
        SpriteComponent* vehicleHealthBar = nullptr;
        SpriteComponent* vehicleHealthBarOutline = nullptr;
        
        // Game Over screen
        TextComponent* gameOverScoreText;
        TextComponent* restartText;
        SpriteComponent* gameOverScreen;

        // Progression related members
        float metersTravelled = 0.0f;
        float lastChunkProgression = 0.0f;

        TextComponent* bulletText = nullptr;
    };
    
    inline GameState g_state;
}