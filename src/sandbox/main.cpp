#include "pch.h"
#include "main.h"

// Override engine settings 
// needs to be done before including app :)
#define ANGLER_WINDOW_WIDTH 1920
#define ANGLER_WINDOW_HEIGHT 1080
#include "application.h"

#pragma comment(linker, "/WHOLEARCHIVE:engine.lib") // DO NOT REMOVE OR AUTOMATIC SCRIPTS & SYSTEM REGISTRATION WILL FAIL, signed Ethan. thanks

#include <DirectXMath.h>

#include "render/os/window.h"
#include "resource-manager.h"
#include "common/time/clock.h"
#include "ecs/components/default-camera-component.h"
#include "render/dx12/objects/mesh.h"
#include "render/renderer/renderer.h"
#include "ecs/components/mesh-component.h"
#include "ecs/components/light-component.h"
#include "ecs/components/particle-emitter-component.h"
#include "ecs/components/sprite-component.h"
#include "ecs/components/text-component.h"
#include "ecs/components/transform-component.h"
#include "render/dx12/objects/static-mesh.h"
#include "render/renderer/cameras/camera-3d.h"
#include "render/dx12/shaders/compute-shader.h"
#include "render/dx12/root-signatures/compute-root-sig.h"
#include "render/dx12/particles-emitter/particles-emitter.h"

using namespace angler;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    Application& app = APPLICATION;
    
    ECS& ecs = app.getECS();
    Renderer& renderer = app.getRenderer();

    std::string resources[24] = {
        "archs",
        "chains",
        "doors&windows",
        "firstfloorbluefabric",
        "firstfloorgreenfabric",
        "firstfloorpillars0",
        "firstfloorpillars1",
        "firstfloorredfabric",
        "flamepots",
        "flowerpots",
        "flowers",
        "groundfloorbluefabric",
        "groundfloorgreenfabric",
        "groundfloorpillars",
        "groundfloorredfabric",
        "interiorroofs",
        "ivy",
        "lanterns",
        "lionhead",
        "mainbase",
        "mainfloors",
        "mainroof",
        "ornament",
        "rods"
    };
    
    // load resources
    for (const auto& element : resources) {
        StaticMesh* mesh = ResourceManager::getMesh("mesh-" + element);
        Texture* texture = ResourceManager::getTexture("tex-" + element);

        ECS::Entity meshEntity = ecs.createEntity();
        ecs.addComponent<TransformComponent>(meshEntity);

        Material* mat = new Material(renderer.getDefault3DShader());
        mat->setProperty("albedoID", texture->getId());
        mat->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        mat->setProperty("metallic", 0.0f);
        mat->setProperty("roughness", 1.0f);
        mat->setProperty("specular", 0.5f);
        
        MeshComponent& meshComp = ecs.addComponent<MeshComponent>(meshEntity);
        meshComp.setMesh(mesh);
        meshComp.setMaterial(mat);
    }

    float flt[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    DirectX::XMFLOAT4X4 world = DirectX::XMFLOAT4X4(flt);
    
    ECS::Entity cameraEntity = ecs.createEntity();
    TransformComponent& cameraTransform = ecs.addComponent<TransformComponent>(cameraEntity);
    Camera3DComponent camera3DComp = ecs.addComponent<Camera3DComponent>(cameraEntity);
    DefaultCameraComponent& defaultCameraController = ecs.addComponent<DefaultCameraComponent>(cameraEntity);
    defaultCameraController.setMode(DefaultCameraComponent::MODE_2);
    cameraTransform.setPosition({-3.0f, 1.0f, 0.0f});
    cameraTransform.lookAt({1.0f, 0.5f, 0.0f});
    
    ECS::Entity sunEntity = ecs.createEntity();
    TransformComponent& sunTransform = ecs.addComponent<TransformComponent>(sunEntity);
    LightComponent& sun = ecs.addComponent<LightComponent>(sunEntity);
    sun.directional();
    sun.setIntensity(0.5f); // Sun config for sponza (night)
    //sun.setIntensity(2.5f); // SUn config for terrain (day)
    
    ECS::Entity spriteEntity = ecs.createEntity();
    TransformComponent& spriteTransform = ecs.addComponent<TransformComponent>(spriteEntity);
    SpriteComponent& sprite = ecs.addComponent<SpriteComponent>(spriteEntity);
    sprite.setSprite(ResourceManager::getSprite("tex-uv-checker", 1, 1));
    sprite.setPosition({25.0f, 25.0f});
    sprite.setColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f));
    sprite.setScale({1, 1});
    sprite.setLayer(1);
    
    ECS::Entity textEntity = ecs.createEntity();
    TransformComponent& textTransform = ecs.addComponent<TransformComponent>(textEntity);
    TextComponent& text = ecs.addComponent<TextComponent>(textEntity);
    text.setFont(ResourceManager::getSprite("font-pixel", 100, 10));
    text.setString("TEST");
    text.setColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    text.setScale(0.5f);
    text.setPosition({200, 100});
    text.setLayer(0);
    
    ECS::Entity pointLightEntity = ecs.createEntity();
    TransformComponent& pointTransform = ecs.addComponent<TransformComponent>(pointLightEntity);
    pointTransform.setPosition({0.0f, 1.0f, 0.0f});
    LightComponent& point = ecs.addComponent<LightComponent>(pointLightEntity);
    point.point();
    point.setIntensity(2.0f);
    MeshComponent& pointMesh = ecs.addComponent<MeshComponent>(pointLightEntity);
    pointMesh.setMesh(ResourceManager::getMesh("mesh-sphere"));
    Material* pointMat = new Material(renderer.getDefault3DShaderUnlit());
    pointMat->setProperty("albedoID", -1); // No texture
    pointMat->setProperty("baseColor", DirectX::XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f));
    pointMat->setProperty("metallic", 0.0f);
    pointMat->setProperty("roughness", 1.0f);
    pointMat->setProperty("specular", 0.0f);
    pointMesh.setMaterial(pointMat);
    
    
    ECS::Entity particleEntity = ecs.createEntity();
    TransformComponent& particleTransform = ecs.addComponent<TransformComponent>(particleEntity);
    particleTransform.setPosition({4.0f, 1.0f, 0.0f});
    LightComponent& particleLight = ecs.addComponent<LightComponent>(particleEntity);
    particleLight.point();
    particleLight.setIntensity(1.0f);
    ParticleEmitterComponent& emitter = ecs.addComponent<ParticleEmitterComponent>(particleEntity);
    emitter.defaultWithMesh(ResourceManager::getMesh("mesh-quad"));
    emitter.addParticleColor({1.0f, 1.0f, 1.0f, 1.0f});
    emitter.setParticlesCount(10);
    emitter.setBillBoardMode(BillBoard::Cylindric);
    emitter.initialize();
    emitter.start();
    
    ECS::Entity terrainEntity = ecs.createEntity();
    TransformComponent& terrTransform = ecs.addComponent<TransformComponent>(terrainEntity);
    terrTransform.setPosition({-40.0f, -29.0f, 65.0f}); // I move the terrain so sponza can be on flat ground
    terrTransform.setScale({5.0f, 5.0f, 5.0f});
    MeshComponent& terrMesh = ecs.addComponent<MeshComponent>(terrainEntity);
    terrMesh.setMesh(ResourceManager::getMesh("mesh-terrain"));
    Texture* terrAlbedo = ResourceManager::getTexture("tex-terrain-diffuse");
    Texture* terrHeightmap = ResourceManager::getTexture("tex-terrain-height");
    Material* terrMat = new Material(renderer.getDefaultHeightmapShader());
    terrMat->setProperty("albedoID", terrAlbedo->getId()); // No texture
    terrMat->setProperty("heightID", terrHeightmap->getId()); // No texture
    terrMat->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    terrMat->setProperty("metallic", 0.0f);
    terrMat->setProperty("roughness", 1.0f);
    terrMat->setProperty("specular", 0.0f);
    terrMesh.setMaterial(terrMat);
    
    return app.run();
}