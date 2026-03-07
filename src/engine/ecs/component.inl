#include "component.h"

#pragma once

namespace angler {
    template <typename T>
    inline Component::id Component::get_id() {
        static id id = get_next_id();
        return id;
    }
}