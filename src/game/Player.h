#pragma once

// Local dependencies
#include "config.h"

// External dependencies
#include <common/time/timer.h>
#include <engine/ecs/system.h>
#include <engine/ecs/components/transform-component.h>

// Forward declarations
struct RailFollower;
namespace angler {
    struct ParticleEmitterComponent;
}

using namespace angler;

struct PlayerComponent : ComponentBase {
    // Player
    int maxHealth = 50;
    int health = 50;
    
    // Ability
    Timer abilityCooldown;
    Timer abilityDuration;

    // Coffin 
    int maxVehicleHealth = 50;
    int vehicleHealth = 50;
    bool repairing = false;
    ECS::Entity QTEWindow;
    float particleUpdateChrono = 0.0f;
    float particleUpdateCooldown = 2.f;

    // Movement
    float minSpeed = VEHICLE_MIN_SPEED;
    float maxSpeed = VEHICLE_MAX_SPEED;
    bool dunking;
    float heightOffset;
    RailFollower* vehicleFollower;
    ParticleEmitterComponent* vehicleEmitter;
    
    // Shooting
    float shotChrono = 0.0f;
    float shotCooldown = 0.1f;
    int maxBulletCount = 12;
    int bulletCount = 12;
    float reloadChrono = 0.0f;
    float reloadTime = 1.5f;
    bool reloading = false;
    ECS::Entity weapon;
};

struct PlayerCameraPivotComponent: ComponentBase {
    float xMax = 0.9f;
    float xMin = -0.9f;
    float rotationFactor = 4.0f;

    ECS::Entity player;
};

void createBullet(PlayerComponent& _player);

void playerCameraUpdateFunction(ECS::Entity _ent, PlayerCameraPivotComponent& _playerCam, TransformComponent& _transform);

SYSTEM_BEGIN(PlayerCameraUpdate, System::UPDATE, PlayerCameraPivotComponent, TransformComponent)
([](ECS::Entity _ent, PlayerCameraPivotComponent& _playerCam, TransformComponent& _transform) {
    playerCameraUpdateFunction(_ent, _playerCam, _transform);
});
SYSTEM_END()

void playerUpdateFunction(ECS::Entity _ent, PlayerComponent& _player, TransformComponent& _transform);

SYSTEM_BEGIN(PlayerUpdate, System::UPDATE, PlayerComponent, TransformComponent)
([](ECS::Entity _ent, PlayerComponent& _player, TransformComponent& _transform) {
    playerUpdateFunction(_ent, _player, _transform);
});
SYSTEM_END()