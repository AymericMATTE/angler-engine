#include "pch.h"
#include "input-device.h"

// External dependencies
#include <algorithm>

KeyValue IInputDevice::getKey(KeyCode _keycode) const {
    return m_final[_keycode];
}

KeyValue IInputDevice::getKeyDown(KeyCode _keycode) const {
    // Pressed: was not active last frame, is active now
    return (m_last[_keycode] == 0.0f && m_final[_keycode] > 0.0f) ? 1.0f : 0.0f;
}

KeyValue IInputDevice::getKeyUp(KeyCode _keycode) const {
    // Released: was active last frame, is not active now
    return (m_last[_keycode] > 0.0f && m_final[_keycode] == 0.0f) ? 1.0f : 0.0f;
}

bool IInputDevice::hasActivity() const {
    return m_activity;
}

void IInputDevice::beginUpdate() {
    m_activity = false;
    // Save current frame's state as last frame's state BEFORE we modify current
    m_last = m_final;
    // Clear current for new frame (optional, but safer)
    m_current = {};
}

void IInputDevice::endUpdate() {
    // Finalize: copy current to final
    m_final = m_current;
}

void IInputDevice::setKey(KeyCode _keycode, KeyValue _value) {
    if (m_current[_keycode] != _value) {
        m_current[_keycode] = _value;
        m_activity = true;
    }
}

void IInputDevice::setKeyRaw(KeyCode _keycode, KeyValue value) {
    if (m_current[_keycode] != value) {
        m_current[_keycode] = value;
        m_activity = true;
    }
}