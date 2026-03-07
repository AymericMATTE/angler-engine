#pragma once

namespace angler {
    class ECS;
}

#include "common/types.h"

#define SYSTEM_BEGIN(name, lifetime, first, ...)    \
    void name##_system(angler::ECS& _ecs);                  \
    static System name##_system_instance = {        \
        lifetime,                                   \
        name##_system                               \
    };                                              \
    inline void name##_system(angler::ECS& _ecs) {          \
        _ecs.foreach<first, __VA_ARGS__>

#define SYSTEM_END()                                \
    }

#define REGISTER_SYSTEM(name)\
    angler::APPLICATION.getECS().registerSystem(name##_system_instance);

struct System {
    enum Lifetime : angler::byte {
        START,
        UPDATE,
        COLLISION
    };

    using SystemFunc = void(*)(angler::ECS&);

    Lifetime lifetime = UPDATE;
    SystemFunc func = nullptr;
};
#include "system.inl"