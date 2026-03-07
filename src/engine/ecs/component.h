#pragma once

#include <cstdint>

namespace angler {
    class Component {
    public:
        Component() = delete;
    
        using id = uint8_t;
    
        template <typename T>
        static id get_id();
    
    private:
        static id get_next_id();
    };
}
#include "component.inl"

