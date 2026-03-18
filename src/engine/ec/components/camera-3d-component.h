#pragma once

#include "render/renderer/cameras/camera-3d.h"
#include "engine/ec/component-base.h"

namespace angler {
    struct Camera3DComponent: public ComponentBase<Camera3DComponent> {
        Camera3DComponent();

        void OnPreRender();
        
        float getFov() const;
        void setFov(float _fov) const;
        
        float getNear() const;
        void setNear(float _near) const;
        
        float getFar() const;
        void setFar(float _far) const;
        
        bool isMain() const;
        
    private:
        bool m_isMain = false;
        mutable Camera3D m_camera3D = {};
        
        friend class Scene;
        friend class Application;
        friend struct Ray;
    };
}