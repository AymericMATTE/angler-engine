#include "pch.h"
#include "enemy.h"

#include "engine/application.h"
#include "engine/ecs/components/state-machine-component.h"
#include "engine/ecs/components/transform-component.h"
#include "engine/ecs/components/mesh-component.h"

#include "engine/resource-manager.h"

#include "game/game-state.h"
#include "game/game-resources.h"
#include "game/Player.h"

#define CRATES_HEIGHT 1.5f

using namespace angler;

/////////////////////////////////
//// General Action
/////////////////////////////////

void updateCooldown(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    EnemyComponent& enemy = ecs.getComponent<EnemyComponent>(*_entity);
    enemy.cooldown -= app.deltaTime();
}

/////////////////////////////////
//// Peak Action
/////////////////////////////////

void beginPeak(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    EnemyComponent& enemy = ecs.getComponent<EnemyComponent>(*_entity);
    enemy.cooldown = EnemyComponent::peakDuration;
}

/////////////////////////////////
//// Shoot Action
/////////////////////////////////

void beginShoot(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();
    
    EnemyComponent& enemy = ecs.getComponent<EnemyComponent>(*_entity);
    
    if (enemy.chunkGeneration != g_state.currentChunk)
        return;
    
    enemy.cooldown = EnemyComponent::shootDuration;

    PlayerComponent& player = ecs.getComponent<PlayerComponent>(g_state.player);

    createEnemyBullet(enemy);
}

/////////////////////////////////
//// Hide Action
/////////////////////////////////

void beginHide(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    EnemyComponent& enemy = ecs.getComponent<EnemyComponent>(*_entity);
    enemy.cooldown = EnemyComponent::hideDuration;

    TransformComponent& transform = ecs.getComponent<TransformComponent>(*_entity);
    enemy.baseY = transform.getPosition().y;
    enemy.hideY = enemy.baseY;
    enemy.hideY -= CRATES_HEIGHT;
}

void updateHide(ECS::Entity* _entity)
{
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    EnemyComponent& enemy = ecs.getComponent<EnemyComponent>(*_entity);
    TransformComponent& transform = ecs.getComponent<TransformComponent>(*_entity);

    enemy.cooldown -= app.deltaTime();

    if (enemy.cooldown < 0.0f)
        enemy.cooldown = 0.0f;

    if (enemy.cooldown > (EnemyComponent::hideDuration / 2.f)) {
        // Going down
        DirectX::XMFLOAT3 goal = transform.getPosition();
        goal.y = std::lerp(enemy.baseY, enemy.hideY, 1.0f - ((enemy.cooldown - EnemyComponent::hideDuration / 2.0f) / (EnemyComponent::hideDuration / 2.0f)));
        transform.setPosition(goal);
    }
    else {
        // Going up
        DirectX::XMFLOAT3 goal = transform.getPosition();
        goal.y = std::lerp(enemy.hideY, enemy.baseY, 1.0f - (enemy.cooldown / (EnemyComponent::hideDuration / 2.0f)));
        transform.setPosition(goal);
    }
}

/////////////////////////////////
//// Death Action
/////////////////////////////////

void beginDeath(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    ecs.destroyEntity(*_entity);
}

/////////////////////////////////
//// General Condition
/////////////////////////////////

bool cooldownCondition(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    EnemyComponent& enemy = ecs.getComponent<EnemyComponent>(*_entity);

    return enemy.cooldown <= 0.0f;
}

/////////////////////////////////
//// Peak Condition
/////////////////////////////////

bool fromPeakCondition(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    StateMachineComponent& sm = ecs.getComponent<StateMachineComponent>(*_entity);

    std::string state = sm.getActualState();

    if (state == "peak")
        return true;

    return false;
}

/////////////////////////////////
//// Shoot Condition
/////////////////////////////////

bool fromShootCondition(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    StateMachineComponent& sm = ecs.getComponent<StateMachineComponent>(*_entity);

    std::string state = sm.getActualState();

    if (state == "shoot")
        return true;

    return false;
}

/////////////////////////////////
//// Hide Condition
/////////////////////////////////

bool fromHideCondition(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    StateMachineComponent& sm = ecs.getComponent<StateMachineComponent>(*_entity);

    std::string state = sm.getActualState();

    if (state == "hide")
        return true;

    return false;
}

/////////////////////////////////
//// Death Condition
/////////////////////////////////

bool deathCondition(ECS::Entity* _entity) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    EnemyComponent& enemy = ecs.getComponent<EnemyComponent>(*_entity);

    return enemy.health <= 0;
}

void addEnemyStateMachine(EnemyComponent& _enemy, bool _shouldHide) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();

    StateMachineComponent& sm = ecs.addComponent<StateMachineComponent>(_enemy.entity);

    sm.addState("peak", { beginPeak, updateCooldown, nullptr });

    if(_shouldHide) {
        sm.addTransition({ {cooldownCondition, fromHideCondition} , "peak" });
        sm.addState("hide", { beginHide, updateHide, nullptr });
        sm.addTransition({ {cooldownCondition, fromShootCondition} , "hide" });
    }
    else {
        sm.addTransition({ {cooldownCondition, fromShootCondition} , "peak" });
    }

    sm.addState("shoot", { beginShoot, updateCooldown, nullptr });
    sm.addTransition({ {cooldownCondition, fromPeakCondition} , "shoot" });

    sm.addState("death", { beginDeath, nullptr, nullptr });
    sm.addTransition({ {deathCondition} , "death" });

    sm.transit("peak");
}

void enemyBulletUpdateFunction(ECS::Entity _ent, EnemyBulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans) {
    float delta = APPLICATION.deltaTime();
    _bullet.lifetime -= delta;

    if (_bullet.lifetime <= 0.0f) {
        ECS& ecs = Application::get().getECS();
        ecs.destroyEntity(_ent);
        return;
    }

    DirectX::XMFLOAT3 forward = _trans.getForward();
    forward.x *= delta * _bullet.speed;
    forward.y *= delta * _bullet.speed;
    forward.z *= delta * _bullet.speed;

    _trans.localTranslate(forward);
}

void enemyBulletCollideFunction(ECS::Entity _ent, CollisionTagComponent& tag, EnemyBulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans) {
    if (_collider.getColliding() == nullptr)
        return;

    ECS& ecs = APPLICATION.getECS();

    if (ecs.hasComponent<PlayerComponent>(_collider.getColliding()->entity)) {
        PlayerComponent& player = ecs.getComponent<PlayerComponent>(_collider.getColliding()->entity);
        ecs.destroyEntity(_ent);

        if (player.dunking) {
            player.vehicleHealth -= _bullet.damage;
            return;
        }

        player.health -= _bullet.damage;
    }
}

void createEnemyBullet(EnemyComponent& _enemy)
{
    ECS& ecs = Application::get().getECS();
    TransformComponent& transform = ecs.getComponent<TransformComponent>(_enemy.entity);
    DirectX::XMFLOAT3 fwd = transform.getForward();
    DirectX::XMFLOAT3 pos = transform.getPosition();

    ECS::Entity bullet = ecs.createEntity();
    TransformComponent& bulletTransform = ecs.addComponent<TransformComponent>(bullet);
    bulletTransform.setPosition({ pos.x + fwd.x, pos.y + fwd.y, pos.z + fwd.z });
    bulletTransform.lookTo(fwd);
    bulletTransform.setScale({ 0.05f, 0.05f, 0.05f });

    ecs.addComponent<EnemyBulletComponent>(bullet).speed = 50.f;
    ecs.addComponent<ColliderComponent>(bullet).SetSphere();

    MeshComponent& bulletMesh = ecs.addComponent<MeshComponent>(bullet);

    bulletMesh.setMesh(g_res.bulletMesh);
    bulletMesh.setMaterial(g_res.bulletMaterial);
}

ECS::Entity spawnEnemy(EnemyType _type, ChunkComponent* _chunk, TransformComponent* _parent, DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT4 _rot) {
    Application& app = APPLICATION;
    ECS& ecs = app.getECS();
    Renderer& renderer = app.getRenderer();
    
    // Create entity
    ECS::Entity enemyEntity = ecs.createEntity();
    TransformComponent& enemyTransform = ecs.addComponent<TransformComponent>(enemyEntity);
    enemyTransform.setParent(_parent);
    enemyTransform.setLocalPosition(_pos);
    enemyTransform.setLocalRotation(_rot);
    
    // Setup AI
    EnemyComponent& enemyAI = ecs.addComponent<EnemyComponent>(enemyEntity);
    enemyAI.chunkGeneration = _chunk->generation;
    switch(_type) {
        case EnemyType::Hidden:
            addEnemyStateMachine(enemyAI, true);
            break;
        case EnemyType::Kamikaze:
            addEnemyStateMachine(enemyAI, false);
            break;
        default:
            break;
    }
    
    // Setup collisions
    ColliderComponent& collideEnemy = ecs.addComponent<ColliderComponent>(enemyEntity);
    collideEnemy.SetBox({0.5f, 1.5f, 0.5f});
    
    // Setup visuals
    MeshComponent& mesh = ecs.addComponent<MeshComponent>(enemyEntity);
    mesh.setMesh(ResourceManager::getMesh("mesh-trashcan"));
    mesh.setMaterial(g_res.enemyMaterial);
    
    return enemyEntity;
}

void enemyUpdateRotation(ECS::Entity _ent, EnemyComponent& _enemyComp, TransformComponent& _transform) {
    //if (_enemyComp.chunkGeneration != g_state.currentChunk)
    //    return;

    ECS& ecs = Application::get().getECS();
    TransformComponent& playerTransform = ecs.getComponent<TransformComponent>(g_state.player);
    

    DirectX::XMFLOAT3 goal = playerTransform.getPosition();
    goal.y = _transform.getPosition().y;

    _transform.lookAt(goal);
}
