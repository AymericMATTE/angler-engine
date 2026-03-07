#include "pch.h"
#include "default-camera-component.h"

namespace angler {
    void DefaultCameraComponent::setMode(Mode _mode) {
        m_mode = _mode;
    }
    
    DefaultCameraComponent::Mode DefaultCameraComponent::getMode() const {
        return m_mode;
    }
}