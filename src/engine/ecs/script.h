#pragma once

#include "common/types.h"

namespace angler {
    class Application;
}

#define SCRIPT_BEGIN(name, lifetime)            \
    void name##_script();                       \
    static Script name##_script_instance = {    \
        lifetime,                               \
        name##_script                           \
    };                                          \
    inline void name##_script() {

#define SCRIPT_END()                            \
    }

#define REGISTER_SCRIPT(name)\
    APPLICATION.getECS().registerScript(name##_script_instance);

struct Script {
    enum Lifetime : angler::byte {
        START,
        UPDATE
    };
    
    using ScriptFunc = void(*)();
    
    Lifetime lifetime = UPDATE;
    ScriptFunc func = nullptr;
};