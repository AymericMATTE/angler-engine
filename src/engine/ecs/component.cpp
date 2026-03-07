#include "pch.h"
#include "component.h"

namespace angler {
    Component::id Component::get_next_id() {
        static id id = 0;
        return id++;
    }
}