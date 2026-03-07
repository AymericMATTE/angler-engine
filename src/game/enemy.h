#pragma once

#define ENEMY_DAMAGE 5

// External dependencies
#include <engine/ecs/component-base.h>
#include <render/renderer/material.h>
#include "engine/ecs/components/collision-tag-component.h"
#include "engine/ecs/components/collider-component.h"

// Forward declarations
namespace angler {
    struct ChunkComponent;
    struct TransformComponent;
}

using namespace angler;

struct EnemyBulletComponent : ComponentBase {
    float speed = 1.5f;
    float lifetime = 3.0f;

    int damage = ENEMY_DAMAGE;
};

void enemyBulletUpdateFunction(ECS::Entity _ent, EnemyBulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans);

SYSTEM_BEGIN(enemyBulletUpdate, System::UPDATE, EnemyBulletComponent, ColliderComponent, TransformComponent)
([](ECS::Entity _ent, EnemyBulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans) {
    enemyBulletUpdateFunction(_ent, _bullet, _collider, _trans);
    });
SYSTEM_END()

void enemyBulletCollideFunction(ECS::Entity _ent, CollisionTagComponent& _tag, EnemyBulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans);

SYSTEM_BEGIN(enemyBulletCollide, System::COLLISION, CollisionTagComponent, EnemyBulletComponent, ColliderComponent, TransformComponent)
([](ECS::Entity _ent, CollisionTagComponent& _tag, EnemyBulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans) {
    enemyBulletCollideFunction(_ent, _tag, _bullet, _collider, _trans);
    });
SYSTEM_END()

enum class EnemyType {
    Hidden,
    Kamikaze
};

struct EnemyComponent : public angler::ComponentBase {
    int health = maxHealth;
    float hideY;
    float baseY;

    float cooldown;
    
    uint chunkGeneration;

    constexpr static inline int maxHealth = 1;
    constexpr static inline float peakDuration = 1.0f;
    constexpr static inline float shootDuration = 1.0f;
    constexpr static inline float hideDuration = 2.0f;
};

void createEnemyBullet(EnemyComponent& _enemy);

ECS::Entity spawnEnemy(EnemyType _type, ChunkComponent * _chunk, TransformComponent * _parent, DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT4 _rot);

void enemyUpdateRotation(ECS::Entity _ent, EnemyComponent& _enemyComp, TransformComponent& _transform);

SYSTEM_BEGIN(enemyRotation, System::UPDATE, EnemyComponent, TransformComponent)
([](ECS::Entity _ent, EnemyComponent& _enemyComp, TransformComponent& _transform) {
    enemyUpdateRotation(_ent, _enemyComp, _transform);
});
SYSTEM_END()

