#include "pch.h"
#include "input-manager.h"

#include <iostream>

InputManager* InputManager::m_instance = nullptr;

void InputManager::init() {
    if (m_instance != nullptr) {
        std::cout << "WARNING : You cannot initialize the InputManager two times ! \n";
        return;
    }

    m_instance = new InputManager();
}

void InputManager::update(float _delta, Rect _viewport) {
    if (m_instance == nullptr) return;
    m_instance->m_keyboard.update(_delta, _viewport);
    m_instance->m_mouse.update(_delta, _viewport);
    m_instance->m_gamepad.update(_delta, _viewport);
}

void InputManager::addAction(const char* _name, const Action& _action) {
    if (m_instance == nullptr) return;
    if (m_instance->m_actions.contains(_name)) return;
    m_instance->m_actions[_name] = _action;
}

void InputManager::addAxis(const char* _name, const Axis& _axis) {
    if (m_instance == nullptr) return;
    if (m_instance->m_axes.contains(_name)) return;
    m_instance->m_axes[_name] = _axis;
}

float InputManager::getAxis(const char* _axis) {
    if (m_instance == nullptr) return false;
    auto it = m_instance->m_axes.find(_axis);
    if (it == m_instance->m_axes.end()) return false;

    KeyValue keyboardValue = (m_instance->m_keyboard.getKey((KeyCode)it->second.keyboardPositiveBinding))
        - (m_instance->m_keyboard.getKey((KeyCode)it->second.keyboardNegativeBinding));

    KeyValue mouseValue = (m_instance->m_mouse.getKey((KeyCode)it->second.mousePositiveBinding))
        - (m_instance->m_mouse.getKey((KeyCode)it->second.mouseNegativeBinding));

    KeyValue gamepadValue = (m_instance->m_gamepad.getKey((KeyCode)it->second.gamepadPositiveBinding))
        - (m_instance->m_gamepad.getKey((KeyCode)it->second.gamepadNegativeBinding));

    return static_cast<float>(keyboardValue + mouseValue + gamepadValue);
}

bool InputManager::getAction(const char* _action) {
    if (m_instance == nullptr) return false;
    auto it = m_instance->m_actions.find(_action);
    if (it == m_instance->m_actions.end()) return false;

    bool keyboardValue = (m_instance->m_keyboard.getKey((KeyCode)it->second.keyboardBinding) > 0.0f);
    
    bool mouseValue = (m_instance->m_mouse.getKey((KeyCode)it->second.mouseBinding) > 0.0f);

    bool gamepadValue = (m_instance->m_gamepad.getKey((KeyCode)it->second.gamepadBinding) > 0.0f);

    return keyboardValue || mouseValue || gamepadValue;
}

bool InputManager::getActionDown(const char* _action) {
    if (m_instance == nullptr) return false;
    auto it = m_instance->m_actions.find(_action);
    if (it == m_instance->m_actions.end()) return false;

    bool keyboardValue = (m_instance->m_keyboard.getKeyDown((KeyCode)it->second.keyboardBinding) > 0.0f);

    bool mouseValue = (m_instance->m_mouse.getKeyDown((KeyCode)it->second.mouseBinding) > 0.0f);

    bool gamepadValue = (m_instance->m_gamepad.getKeyDown((KeyCode)it->second.gamepadBinding) > 0.0f);

    return keyboardValue || mouseValue || gamepadValue;
}

bool InputManager::getActionUp(const char* _action) {
    if (m_instance == nullptr) return false;
    auto it = m_instance->m_actions.find(_action);
    if (it == m_instance->m_actions.end()) return false;

    bool keyboardValue = (m_instance->m_keyboard.getKeyUp((KeyCode)it->second.keyboardBinding) > 0.0f);

    bool mouseValue = (m_instance->m_mouse.getKeyUp((KeyCode)it->second.mouseBinding) > 0.0f);

    bool gamepadValue = (m_instance->m_gamepad.getKeyUp((KeyCode)it->second.gamepadBinding) > 0.0f);

    return keyboardValue || mouseValue || gamepadValue;
}

DirectX::XMFLOAT2 InputManager::getMousePos() {

}