#include "pch.h"
#include "sandbox-scene.h"

#include "engine/application.h"
#include "engine/resource-manager.h"
#include "engine/inputs/input-manager.h"

#include "engine/ec/components/collider-component.h"
#include "engine/ec/components/default-camera-component.h"
#include "engine/ec/components/mesh-component.h"
#include "engine/ec/components/light-component.h"
#include "engine/ec/components/text-component.h"
#include "engine/ec/components/sprite-component.h"
#include "engine/ec/components/particle-emitter-component.h"
#include "engine/ec/components/sprite-animator-3d-component.h"

#include "render/dx12/objects/static-mesh.h"

void SetupInputs() {
    InputManager::addAxis("default_cam_vertical", {
        Keyboard::Key::Z,
        Keyboard::Key::S,
        Mouse::Key::NONE,
        Mouse::Key::NONE,
        Gamepad::Key::LEFT_STICK_UP,
        Gamepad::Key::LEFT_STICK_DOWN,
        });

    InputManager::addAxis("default_cam_horizontal", {
        Keyboard::Key::D,
        Keyboard::Key::Q,
        Mouse::Key::NONE,
        Mouse::Key::NONE,
        Gamepad::Key::LEFT_STICK_RIGHT,
        Gamepad::Key::LEFT_STICK_LEFT,
        });

    InputManager::addAxis("default_cam_elevation", {
        Keyboard::Key::SPACE,
        Keyboard::Key::LCONTROL,
        Mouse::Key::NONE,
        Mouse::Key::NONE,
        Gamepad::Key::RIGHT_TRIGGER_AXIS,
        Gamepad::Key::LEFT_TRIGGER_AXIS
        });

    InputManager::addAxis("default_cam_mouse_x", {
        Keyboard::Key::NONE,
        Keyboard::Key::NONE,

        Mouse::Key::DELTA_RIGHT,
        Mouse::Key::DELTA_LEFT,
        Gamepad::Key::RIGHT_STICK_RIGHT,
        Gamepad::Key::RIGHT_STICK_LEFT
        });

    InputManager::addAxis("default_cam_mouse_y", {
        Keyboard::Key::NONE,
        Keyboard::Key::NONE,

        Mouse::Key::DELTA_UP,
        Mouse::Key::DELTA_DOWN,
        Gamepad::Key::RIGHT_STICK_UP,
        Gamepad::Key::RIGHT_STICK_DOWN
        });

    InputManager::addAction("default_cam_accelerate", {
        Keyboard::Key::LSHIFT,
        Mouse::Key::NONE,
        Gamepad::Key::LEFT_THUMB
        });

    InputManager::addAction("default_cam_lock_cursor", {
        Keyboard::Key::ESCAPE,
        Mouse::Key::NONE,
        Gamepad::Key::NONE
        });

    InputManager::addAction("default_cam_look_around", {
        Keyboard::Key::NONE,
        Mouse::Key::BUTTON_RIGHT,
        Gamepad::Key::NONE
        });
}

void SandboxScene::OnStart() {
    SetupInputs();

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

    angler::Renderer& renderer = angler::Application::get().getRenderer();

    angler::GameObject* object = CreateGameObject();
    object->setPosition({ 0.0f, 1.0f, -3.0f });
    object->addComponent<angler::ColliderComponent>()->SetSphere();
    angler::Camera3DComponent* camera = object->addComponent<angler::Camera3DComponent>();
    object->addComponent<angler::DefaultCameraComponent>()->setMode(angler::DefaultCameraComponent::MODE_2);

    SetMainCamera(camera);
    
    angler::GameObject* sun = CreateGameObject();
    angler::LightComponent* sunlight = sun->addComponent<angler::LightComponent>();
    sunlight->directional();
    sunlight->setIntensity(2.5f); // Sun config for sponza (day)

    // load resources
    for (const auto& element : resources) {
        angler::StaticMesh* mesh = angler::ResourceManager::getMesh("mesh-" + element);
        angler::Texture* texture = angler::ResourceManager::getTexture("tex-" + element);

        angler::GameObject* meshEntity = CreateGameObject();

        angler::Material* mat = new angler::Material(renderer.getDefault3DShader());
        mat->setProperty("albedoID", texture->getId());
        mat->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        mat->setProperty("metallic", 0.0f);
        mat->setProperty("roughness", 1.0f);
        mat->setProperty("specular", 0.5f);

        angler::MeshComponent* meshComp = meshEntity->addComponent<angler::MeshComponent>();
        meshComp->setMesh(mesh);
        meshComp->setMaterial(mat);
    }

    /*angler::GameObject* spriteObject = CreateGameObject();
    angler::SpriteComponent* sprite = spriteObject->addComponent<angler::SpriteComponent>();
    sprite->setSprite(angler::ResourceManager::getSprite("tex-uv-checker", 1, 1));
    sprite->setPosition({ 25.0f, 25.0f });
    sprite->setColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f));
    sprite->setScale({ 1, 1 });
    sprite->setLayer(0);*/

    angler::GameObject* textObject = CreateGameObject();
    angler::TextComponent* text = textObject->addComponent<angler::TextComponent>();
    text->setFont(angler::ResourceManager::getSprite("font-pixel", 100, 10));
    text->setString("TEST");
    text->setColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    text->setScale(0.5f);
    text->setPosition({ 200, 100 });
    text->setLayer(1);

    angler::GameObject* pointLightObject = CreateGameObject();
    pointLightObject->setPosition({ 0.0f, 1.0f, 0.0f });
    angler::LightComponent* point = pointLightObject->addComponent<angler::LightComponent>();
    point->point();
    point->setIntensity(2.0f);
    angler::MeshComponent* pointMesh = pointLightObject->addComponent<angler::MeshComponent>();
    pointMesh->setMesh(angler::ResourceManager::getMesh("mesh-sphere"));
    angler::Material* pointMat = new angler::Material(renderer.getDefault3DShaderUnlit());
    pointMat->setProperty("albedoID", -1); // No texture
    pointMat->setProperty("baseColor", DirectX::XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f));
    pointMat->setProperty("metallic", 0.0f);
    pointMat->setProperty("roughness", 1.0f);
    pointMat->setProperty("specular", 0.0f);
    pointMesh->setMaterial(pointMat);
    pointLightObject->addComponent<angler::ColliderComponent>()->SetSphere();

    angler::Material* textureMat = new angler::Material(renderer.getDefault3DShader());
    textureMat->setProperty("albedoID", angler::ResourceManager::getTexture("tex-wooden-plank")->getId()); // No texture
    textureMat->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    textureMat->setProperty("metallic", 0.0f);
    textureMat->setProperty("roughness", 1.0f);
    textureMat->setProperty("specular", 0.0f);

    angler::GameObject* SpriteObject3D = CreateGameObject();
    angler::MeshComponent* spriteMesh = SpriteObject3D->addComponent<angler::MeshComponent>();
    angler::SpriteAnimator3DComponent* animator = SpriteObject3D->addComponent<angler::SpriteAnimator3DComponent>();
    spriteMesh->setMesh(angler::ResourceManager::getMesh("mesh-quad"));
    animator->setSprite(angler::ResourceManager::getSprite("tex-uv-checker", 64, 8));

    SpriteAnimation defaultAnim;
    defaultAnim.frames.push_back({ {0, 0}, 2 });
    defaultAnim.frames.push_back({ {0, 1}, 0.5 });
    defaultAnim.frames.push_back({ {1, 0}, 0.5 });
    defaultAnim.frames.push_back({ {1, 1}, 0.5 });
    animator->addAnimation(defaultAnim, "default");
    animator->play("default");

    //SpriteObject3D->setParent(object);
    SpriteObject3D->setLocalPosition({ 0, 1, 1 });
    SpriteObject3D->localRotate({ 0, 0, -3.1415/2 });

    angler::GameObject* particleObject = CreateGameObject();
    particleObject->setPosition({ 4.0f, 1.0f, 0.0f });
    angler::LightComponent* particleLight = particleObject->addComponent<angler::LightComponent>();
    particleLight->point();
    particleLight->setIntensity(1.0f);
    angler::ParticleEmitterComponent* emitter = particleObject->addComponent<angler::ParticleEmitterComponent>();
    emitter->defaultWithMesh(angler::ResourceManager::getMesh("mesh-quad"));
    emitter->addParticleColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    emitter->setBillBoardMode(angler::BillBoard::Cylindric);
    emitter->setParticlesCount(10);
    emitter->initialize();
    emitter->start();

    angler::GameObject* terrain = CreateGameObject();
    terrain->setPosition({ -40.0f, -29.0f, 65.0f }); // I move the terrain so sponza can be on flat ground
    terrain->setScale({ 5.0f, 5.0f, 5.0f });
    angler::MeshComponent* terrMesh = terrain->addComponent<angler::MeshComponent>();
    terrMesh->setMesh(angler::ResourceManager::getMesh("mesh-terrain"));
    angler::Texture* terrAlbedo = angler::ResourceManager::getTexture("tex-terrain-diffuse");
    angler::Texture* terrHeightmap = angler::ResourceManager::getTexture("tex-terrain-height");
    angler::Material* terrMat = new angler::Material(renderer.getDefaultHeightmapShader());
    terrMat->setProperty("albedoID", terrAlbedo->getId()); // No texture
    terrMat->setProperty("heightID", terrHeightmap->getId()); // No texture
    terrMat->setProperty("baseColor", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    terrMat->setProperty("metallic", 0.0f);
    terrMat->setProperty("roughness", 1.0f);
    terrMat->setProperty("specular", 0.0f);
    terrMesh->setMaterial(terrMat);
}