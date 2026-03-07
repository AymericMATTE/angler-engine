#include "pch.h"
#include "bullet.h"
#include "engine/inputs/input-manager.h"
#include "enemy.h"
#include "crate-component.h"

void bulletUpdateFunction(ECS::Entity _ent, BulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans) {
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

void bulletCollideFunction(ECS::Entity _ent, CollisionTagComponent& tag, BulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans) {
    if (_collider.getColliding() == nullptr) 
        return;

    ECS& ecs = APPLICATION.getECS();

    if (ecs.hasComponent<CrateComponent>(_collider.getColliding()->entity)) {
        ecs.destroyEntity(_ent);
    }
    else if (ecs.hasComponent<EnemyComponent>(_collider.getColliding()->entity)) {
        EnemyComponent& enemy = ecs.getComponent<EnemyComponent>(_collider.getColliding()->entity);
        enemy.health -= _bullet.damage;
        ecs.destroyEntity(_ent);
    }
}
