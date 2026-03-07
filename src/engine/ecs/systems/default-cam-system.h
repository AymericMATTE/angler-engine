#pragma once

#include "../system.h"

#include "../../application.h"
#include "../../inputs/input-manager.h"
#include "../components/transform-component.h"
#include "ecs/components/default-camera-component.h"

namespace angler {
    inline void DefaultCameraComponent_updateMovements(TransformComponent& _trans) {
        const float delta = APPLICATION.deltaTime();
        const float speed = InputManager::getAction("default_cam_accelerate") ? 8.0f : 4.0f;
        const float moveFactor = delta * speed;

        // Mouse look
        const float mouseX = InputManager::getAxis("default_cam_mouse_x");
        const float mouseY = InputManager::getAxis("default_cam_mouse_y");

        _trans.rotate({ 0.0f,  mouseX * delta * 4.0f, 0.0f });
        _trans.localRotate({ -mouseY * delta * 4.0f, 0.0f, 0.0f });

        // Movement input
        const float vertical   = InputManager::getAxis("default_cam_vertical");
        const float horizontal = InputManager::getAxis("default_cam_horizontal");
        const float elevation  = InputManager::getAxis("default_cam_elevation");

        DirectX::XMFLOAT3 forward = _trans.getForward();
        DirectX::XMFLOAT3 right   = _trans.getRight();
        DirectX::XMFLOAT3 up      = _trans.getUp();

        DirectX::XMFLOAT3 movement = {
            (forward.x * vertical + right.x * horizontal + up.x * elevation) * moveFactor,
            (forward.y * vertical + right.y * horizontal + up.y * elevation) * moveFactor,
            (forward.z * vertical + right.z * horizontal + up.z * elevation) * moveFactor
        };

        _trans.translate(movement);
    }
    
    inline void DefaultCameraComponent_updateMode1(TransformComponent& _trans) {
        static bool locked = false;

        if (InputManager::getActionDown("default_cam_lock_cursor")) {
            locked = !locked;
            locked ? Mouse::lock() : Mouse::unlock();
        }

        if (!locked) return;

        DefaultCameraComponent_updateMovements(_trans);
    }
    
    inline void DefaultCameraComponent_updateMode2(TransformComponent& _trans) {
        if (InputManager::getActionDown("default_cam_look_around"))
            Mouse::lock();
        if (InputManager::getActionUp("default_cam_look_around"))
            Mouse::unlock();
        
        const bool look = InputManager::getAction("default_cam_look_around");
        if (!look) return;
        
        DefaultCameraComponent_updateMovements(_trans);
    }
    
    SYSTEM_BEGIN(UpdateDefaultCamera, System::UPDATE, DefaultCameraComponent, TransformComponent)
    ([](ECS::Entity _ent, DefaultCameraComponent& _def, TransformComponent& _trans) {
        switch (_def.getMode()) {
        case DefaultCameraComponent::MODE_1:
            DefaultCameraComponent_updateMode1(_trans);
            break;
        case DefaultCameraComponent::MODE_2:
            DefaultCameraComponent_updateMode2(_trans);
            break;
        }
    });
    SYSTEM_END()
}