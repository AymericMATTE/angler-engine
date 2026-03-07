#pragma once

#include "application.h"
#include "engine/ecs/ecs.h"
#include "engine/ecs/components/transform-component.h"
#include "rail-follower-component.h"

using namespace angler;

void railSystemFunction(ECS::Entity _entity, RailFollower& _follower, TransformComponent& _transform);

SYSTEM_BEGIN(RailSystem, System::UPDATE, RailFollower, TransformComponent)
([](ECS::Entity _entity, RailFollower& _follower, TransformComponent& _transform) {
    railSystemFunction(_entity, _follower, _transform);
});
SYSTEM_END()

void chunkCleanupSystemFunction(ECS::Entity _ent, RailFollower& _follower);

SYSTEM_BEGIN(ChunkCleanupSystem, System::UPDATE, RailFollower)
([](ECS::Entity _ent, RailFollower& _follower) {
    chunkCleanupSystemFunction(_ent, _follower);
});
SYSTEM_END()