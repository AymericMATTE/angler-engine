#pragma once

#include "../script.h"
#include "inputs/input-manager.h"

SCRIPT_BEGIN(SetupDefaultCameraInputs, Script::Lifetime::START)
    InputManager::addAxis("default_cam_vertical", {
        Keyboard::Key::Z,
        Keyboard::Key::S,
        Mouse::Key::NONE,
        Mouse::Key::NONE,
        Gamepad::Key::LEFT_STICK_UP,
        Gamepad::Key::LEFT_STICK_DOWN,
    });

    InputManager::addAxis("default_cam_horizontal", {
        Keyboard::Key::D,
        Keyboard::Key::Q,
        Mouse::Key::NONE,
        Mouse::Key::NONE,
        Gamepad::Key::LEFT_STICK_RIGHT,
        Gamepad::Key::LEFT_STICK_LEFT,
    });
    
    InputManager::addAxis("default_cam_elevation", {
        Keyboard::Key::SPACE,
        Keyboard::Key::LCONTROL,
        Mouse::Key::NONE,
        Mouse::Key::NONE,
        Gamepad::Key::RIGHT_TRIGGER_AXIS,
        Gamepad::Key::LEFT_TRIGGER_AXIS
    });

    InputManager::addAxis("default_cam_mouse_x", {
        Keyboard::Key::NONE,
        Keyboard::Key::NONE,
                
        Mouse::Key::DELTA_RIGHT,
        Mouse::Key::DELTA_LEFT,
        Gamepad::Key::RIGHT_STICK_RIGHT,
        Gamepad::Key::RIGHT_STICK_LEFT
    });

    InputManager::addAxis("default_cam_mouse_y", {
        Keyboard::Key::NONE,
        Keyboard::Key::NONE,
    
        Mouse::Key::DELTA_UP,
        Mouse::Key::DELTA_DOWN,
        Gamepad::Key::RIGHT_STICK_UP,
        Gamepad::Key::RIGHT_STICK_DOWN
    });
    
    InputManager::addAction("default_cam_accelerate", {
        Keyboard::Key::LSHIFT,
        Mouse::Key::NONE,
        Gamepad::Key::LEFT_THUMB
    });
    
    InputManager::addAction("default_cam_lock_cursor", {
        Keyboard::Key::ESCAPE,
        Mouse::Key::NONE,
        Gamepad::Key::NONE
    });

    InputManager::addAction("default_cam_look_around", {
        Keyboard::Key::NONE,
        Mouse::Key::BUTTON_RIGHT,
        Gamepad::Key::NONE
    });
SCRIPT_END()