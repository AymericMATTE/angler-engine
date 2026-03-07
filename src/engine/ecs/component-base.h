#pragma once

#include "ecs.h"
namespace angler {
    class ComponentBase {
    public:
        ECS::Entity entity = ECS::invalidEntity;
    };
}

