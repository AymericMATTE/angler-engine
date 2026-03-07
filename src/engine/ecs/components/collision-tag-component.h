#pragma once

#include "common/types.h"
#include "engine/ecs/component-base.h"

namespace angler {
    struct CollisionTagComponent: public ComponentBase {
        bool test = true;
    };
}