#pragma once

#include "common/types.h"
#include "engine/ecs/component-base.h"

namespace angler {
    struct DefaultCameraComponent: public ComponentBase {
        enum Mode : byte{
            MODE_1,
            MODE_2,
        };
        
        void setMode(Mode _mode);
        Mode getMode() const;
        
    private:
        Mode m_mode;
    };
}