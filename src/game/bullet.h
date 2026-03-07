#pragma once

#include "engine/ecs/system.h"
#include "engine/application.h"
#include "engine/ecs/components/collider-component.h"
#include "engine/ecs/components/transform-component.h"
#include "engine/ecs/components/collision-tag-component.h"

using namespace angler;

struct BulletComponent : ComponentBase {
    float speed = 2.0f;
    float lifetime = 5.0f;

    int damage = 1;
};

void bulletUpdateFunction(ECS::Entity _ent, BulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans);

SYSTEM_BEGIN(BulletUpdate, System::UPDATE, BulletComponent, ColliderComponent, TransformComponent)
([](ECS::Entity _ent, BulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans) {
    bulletUpdateFunction(_ent, _bullet, _collider, _trans);
    });
SYSTEM_END()

void bulletCollideFunction(ECS::Entity _ent, CollisionTagComponent& tag, BulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans);

SYSTEM_BEGIN(BulletCollide, System::COLLISION, CollisionTagComponent, BulletComponent, ColliderComponent, TransformComponent)
([](ECS::Entity _ent, CollisionTagComponent& _tag, BulletComponent& _bullet, ColliderComponent& _collider, TransformComponent& _trans) {
    bulletCollideFunction(_ent, _tag, _bullet, _collider, _trans);
    });
SYSTEM_END()