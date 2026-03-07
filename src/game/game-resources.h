#pragma once

// Local dependencies
#include "config.h"

// External dependencies
// #include ""

// Forward declarations
namespace angler {
    class Shader;
    class Texture;
    class Material;
    class Renderer;
    class StaticMesh;
    struct Font;
    struct SpriteSheet;
}

namespace angler {
    struct ChunkResource {
        StaticMesh* bodyMesh = nullptr;
        Texture* bodyTexture = nullptr;
        Material* bodyMaterial = nullptr;
        
        StaticMesh* waterMesh = nullptr;
    };
    
    struct GameResources {
        // Player related resources
        StaticMesh* coffinMesh = nullptr;
        Texture* coffinTexture = nullptr;
        Material* coffinMaterial = nullptr;
        
        StaticMesh* gunMesh = nullptr;
        Texture* gunTexture = nullptr;
        Material* gunMaterial = nullptr;
        
        StaticMesh* bulletMesh = nullptr;
        Texture* bulletTexture = nullptr;
        Material* bulletMaterial = nullptr;
        
        // UI related resources
        Font* mainFont = nullptr;
        SpriteSheet* crosshair = nullptr;
        SpriteSheet* healthbar = nullptr;
        SpriteSheet* healthbarOutline = nullptr;
        SpriteSheet* vehicleHealthbar = nullptr;
        SpriteSheet* vehicleHealthbarOutline = nullptr;

        Texture* enemyTexture = nullptr;
        Material* enemyMaterial = nullptr;

        // QTE UI resources
        SpriteSheet* popupSprite = nullptr;
        
        // Generation related resources
        Shader* standardShader = nullptr;
        Shader* waterShader = nullptr;
        
        Texture* waterTexture = nullptr;
        Material* waterMaterial = nullptr;

        StaticMesh* crateMesh = nullptr;
        Texture* crateTexture = nullptr;
        Material* crateMaterial = nullptr;
        
        ChunkResource chunkResources[NUM_CHUNKS_TYPE];
        
        void load(Renderer& _renderer);
    };

    inline GameResources g_res;
}