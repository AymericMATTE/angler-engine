#include "pch.h"
#include "default-camera-component.h"
#include "application.h"
#include "engine/inputs/input-manager.h"

namespace angler {
    void DefaultCameraComponent::OnUpdate() {
        switch (getMode()) {
        case DefaultCameraComponent::MODE_1:
            updateMode1();
            break;
        case DefaultCameraComponent::MODE_2:
            updateMode2();
            break;
        }
    }

    void DefaultCameraComponent::updateMovements()
    {
        GameObject* owner = getOwner();
        const float delta = Application::get().deltaTime();
        const float speed = InputManager::getAction("default_cam_accelerate") ? 8.0f : 4.0f;
        const float moveFactor = delta * speed;

        // Mouse look
        const float mouseX = InputManager::getAxis("default_cam_mouse_x");
        const float mouseY = InputManager::getAxis("default_cam_mouse_y");

        owner->rotate({ 0.0f,  mouseX * delta * 4.0f, 0.0f });
        owner->localRotate({ -mouseY * delta * 4.0f, 0.0f, 0.0f });

        // Movement input
        const float vertical = InputManager::getAxis("default_cam_vertical");
        const float horizontal = InputManager::getAxis("default_cam_horizontal");
        const float elevation = InputManager::getAxis("default_cam_elevation");

        DirectX::XMFLOAT3 forward = owner->getForward();
        DirectX::XMFLOAT3 right = owner->getRight();
        DirectX::XMFLOAT3 up = owner->getUp();

        DirectX::XMFLOAT3 movement = {
            (forward.x * vertical + right.x * horizontal + up.x * elevation) * moveFactor,
            (forward.y * vertical + right.y * horizontal + up.y * elevation) * moveFactor,
            (forward.z * vertical + right.z * horizontal + up.z * elevation) * moveFactor
        };

        owner->translate(movement);
    }

    void DefaultCameraComponent::updateMode1()
    {
        static bool locked = false;

        if (InputManager::getActionDown("default_cam_lock_cursor")) {
            locked = !locked;
            locked ? Mouse::lock() : Mouse::unlock();
        }

        if (!locked) return;
        updateMovements();
    }

    void DefaultCameraComponent::updateMode2()
    {
        if (InputManager::getActionDown("default_cam_look_around"))
            Mouse::lock();
        if (InputManager::getActionUp("default_cam_look_around"))
            Mouse::unlock();

        const bool look = InputManager::getAction("default_cam_look_around");
        if (!look) return;
        updateMovements();
    }

    void DefaultCameraComponent::setMode(Mode _mode) {
        m_mode = _mode;
    }
    
    DefaultCameraComponent::Mode DefaultCameraComponent::getMode() const {
        return m_mode;
    }
}