#include "pch.h"
#include "game-resources.h"

#include "resource-manager.h"
#include "common/common.h"
#include "common/utils/os.h"
#include "generation/lvl-generator.h"
#include "render/renderer/material.h"
#include "render/dx12/objects/sprite-sheet.h"
#include "render/renderer/renderer.h"

namespace angler {
    void GameResources::load(Renderer& _renderer) {
        // Set standard shader
        standardShader = _renderer.getDefault3DShader();
        
        // Load coffin resources
        coffinMesh = ResourceManager::getMesh("mesh-coffin");
        
        coffinTexture = ResourceManager::getTexture("tex-coffin");
        coffinMaterial = new Material(standardShader);
        coffinMaterial->setProperty("albedoID", coffinTexture->getId());
        coffinMaterial->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        coffinMaterial->setProperty("metallic", 0.0f);
        coffinMaterial->setProperty("roughness", 1.0f);
        coffinMaterial->setProperty("specular", 0.5f);
        
        // Load enemy resources
        enemyTexture = ResourceManager::getTexture("tex-trashcan");
        enemyMaterial = new Material(standardShader);
        enemyMaterial->setProperty("albedoID", enemyTexture->getId());
        enemyMaterial->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        enemyMaterial->setProperty("metallic", 1.0f);
        enemyMaterial->setProperty("roughness", 0.0f);
        enemyMaterial->setProperty("specular", 0.5f);


        // Load gun resources
        gunMesh = ResourceManager::getMesh("mesh-gun");
        
        gunTexture = ResourceManager::getTexture("tex-gun");
        gunMaterial = new Material(standardShader);
        gunMaterial->setProperty("albedoID", gunTexture->getId());
        gunMaterial->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        gunMaterial->setProperty("metallic", 0.0f);
        gunMaterial->setProperty("roughness", 1.0f);
        gunMaterial->setProperty("specular", 0.5f);
        
        
        // Load bullet resources
        bulletMesh = ResourceManager::getMesh("mesh-rat");
        
        bulletTexture = ResourceManager::getTexture("tex-rat");
        bulletMaterial = new Material(standardShader);
        bulletMaterial->setProperty("albedoID", bulletTexture->getId());
        bulletMaterial->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        bulletMaterial->setProperty("metallic", 0.0f);
        bulletMaterial->setProperty("roughness", 1.0f);
        bulletMaterial->setProperty("specular", 0.5f);
        
        
        // Load UI resources
        mainFont = reinterpret_cast<Font*>(ResourceManager::getSprite("font-pixel", 126 - 32, 10));
        crosshair = ResourceManager::getSprite("crosshair", 1, 1);
        healthbar = ResourceManager::getSprite("tex-player-healthbar-fill", 1, 1);
        healthbarOutline = ResourceManager::getSprite("tex-player-healthbar-bg", 1, 1);
        vehicleHealthbar = ResourceManager::getSprite("tex-coffin-healthbar-fill", 1, 1);
        vehicleHealthbarOutline = ResourceManager::getSprite("tex-coffin-healthbar-bg", 1, 1);
        popupSprite = ResourceManager::getSprite("tex-wooden-plank", 1, 1); // TODO: window sprite
        
        // Load water shader
        waterShader = new Shader();
        waterShader->initialize(
            stringToWString(os::getFileInDirectory(RES_DIR,"3d-water.hlsl")).c_str(),
            stringToWString(os::getFileInDirectory(RES_DIR,"3d-water.hlsl")).c_str(),
            _renderer.get3DRootSig(),
            _renderer.get3DInputLayout()
        );
        
        // Load water material
        waterTexture = ResourceManager::getTexture("tex-water");
        waterMaterial = new Material(waterShader);
        waterMaterial->setProperty("albedoID", waterTexture->getId());
        waterMaterial->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        waterMaterial->setProperty("metallic", 0.0f);
        waterMaterial->setProperty("roughness", 1.0f);
        waterMaterial->setProperty("specular", 0.5f);

        // Load crate resources
        crateTexture = ResourceManager::getTexture("tex-crate");
        crateMesh = ResourceManager::getMesh("mesh-crate");
        crateMaterial = new Material(standardShader);
        crateMaterial->setProperty("albedoID", crateTexture->getId());
        crateMaterial->setProperty("baseColor", DirectX::XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
        crateMaterial->setProperty("metallic", 0.0f);
        crateMaterial->setProperty("roughness", 1.0f);
        crateMaterial->setProperty("specular", 0.5f);

        // Load chunk resources
        for (int i = 0; i < NUM_CHUNKS_TYPE; ++i) {
            const ChunkType& type = chunkTypes[i];
            ChunkResource& res = chunkResources[i];
        
            // Load meshes
            res.bodyMesh = ResourceManager::getMesh("mesh-" + std::string(type.resourceName));
            res.waterMesh = ResourceManager::getMesh("mesh-" + std::string(type.resourceName) + "-water");

            // Load materials
            res.bodyTexture = ResourceManager::getTexture("tex-" + std::string(type.resourceName));
            res.bodyMaterial = new Material(standardShader);
            res.bodyMaterial->setProperty("albedoID", res.bodyTexture->getId());
            res.bodyMaterial->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
            res.bodyMaterial->setProperty("metallic", 0.0f);
            res.bodyMaterial->setProperty("roughness", 1.0f);
            res.bodyMaterial->setProperty("specular", 0.5f);
        }
    }
}