#pragma once

#include "common/types.h"
#include "engine/ec/component-base.h"

namespace angler {
    struct DefaultCameraComponent: public ComponentBase<DefaultCameraComponent> {
        enum Mode : byte{
            MODE_1,
            MODE_2,
        };

        void OnUpdate();
        void updateMovements();
        void updateMode1();
        void updateMode2();

        void setMode(Mode _mode);
        Mode getMode() const;
        
    private:
        Mode m_mode;
    };
}