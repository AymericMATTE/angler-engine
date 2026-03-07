#include "pch.h"
#include "gamepad-device.h"

#include <algorithm>

#include <windows.h>
#include <xinput.h>

#pragma comment(lib, "xinput.lib")

static constexpr float LEFT_THUMB_DEADZONE = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / 32767.0f;
static constexpr float RIGHT_THUMB_DEADZONE = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE / 32767.0f;
static constexpr float TRIGGER_THRESHOLD = XINPUT_GAMEPAD_TRIGGER_THRESHOLD / 255.0f;

float Gamepad::normalizeStick(short _value, float _deadzone) {
	float normalized = static_cast<float>(_value) / 32767.0f;

	// Deadzone
	if (std::abs(normalized) < _deadzone) {
		return 0.0f;
	}

	// Scale to range
	float sign = (normalized > 0.0f) ? 1.0f : -1.0f;
	return (normalized - sign * _deadzone) / (1.0f - _deadzone);
}

void Gamepad::update(float _dt, Rect _viewport) {
	beginUpdate();

	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	// Only first controller
	DWORD result = XInputGetState(0, &state);
	m_connected = (result == ERROR_SUCCESS);

	if (!m_connected) {
		endUpdate();
		return;
	}

	XINPUT_GAMEPAD& pad = state.Gamepad;

	setKey(static_cast<KeyCode>(Key::A), (pad.wButtons & XINPUT_GAMEPAD_A) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::B), (pad.wButtons & XINPUT_GAMEPAD_B) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::X), (pad.wButtons & XINPUT_GAMEPAD_X) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::Y), (pad.wButtons & XINPUT_GAMEPAD_Y) ? 1.0f : 0.0f);

	setKey(static_cast<KeyCode>(Key::LEFT_SHOULDER), (pad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::RIGHT_SHOULDER), (pad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 1.0f : 0.0f);

	setKey(static_cast<KeyCode>(Key::DPAD_UP), (pad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::DPAD_DOWN), (pad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::DPAD_LEFT), (pad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::DPAD_RIGHT), (pad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 1.0f : 0.0f);

	setKey(static_cast<KeyCode>(Key::LEFT_THUMB), (pad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::RIGHT_THUMB), (pad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 1.0f : 0.0f);

	setKey(static_cast<KeyCode>(Key::START), (pad.wButtons & XINPUT_GAMEPAD_START) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::BACK), (pad.wButtons & XINPUT_GAMEPAD_BACK) ? 1.0f : 0.0f);

	// Macro for some xbox one specific shit..
#ifdef XINPUT_GAMEPAD_GUIDE
	setKey(static_cast<KeyCode>(Key::GUIDE), (pad.wButtons & XINPUT_GAMEPAD_GUIDE) ? 1.0f : 0.0f);
#else
	setKey(static_cast<KeyCode>(Key::GUIDE), 0.0f);
#endif

	float leftTrigger = static_cast<float>(pad.bLeftTrigger) / 255.0f;
	float rightTrigger = static_cast<float>(pad.bRightTrigger) / 255.0f;

	setKey(static_cast<KeyCode>(Key::LEFT_TRIGGER_AXIS), leftTrigger);
	setKey(static_cast<KeyCode>(Key::RIGHT_TRIGGER_AXIS), rightTrigger);
	setKey(static_cast<KeyCode>(Key::LEFT_TRIGGER_BTN), (leftTrigger > TRIGGER_THRESHOLD) ? 1.0f : 0.0f);
	setKey(static_cast<KeyCode>(Key::RIGHT_TRIGGER_BTN), (rightTrigger > TRIGGER_THRESHOLD) ? 1.0f : 0.0f);

	float leftX = normalizeStick(pad.sThumbLX, LEFT_THUMB_DEADZONE);
	float leftY = normalizeStick(pad.sThumbLY, LEFT_THUMB_DEADZONE);

	setKey(static_cast<KeyCode>(Key::LEFT_STICK_LEFT), (leftX < 0.0f) ? -leftX : 0.0f);
	setKey(static_cast<KeyCode>(Key::LEFT_STICK_RIGHT), (leftX > 0.0f) ? leftX : 0.0f);
	setKey(static_cast<KeyCode>(Key::LEFT_STICK_UP), (leftY > 0.0f) ? leftY : 0.0f); // Somehow positive ?
	setKey(static_cast<KeyCode>(Key::LEFT_STICK_DOWN), (leftY < 0.0f) ? -leftY : 0.0f);

	float rightX = normalizeStick(pad.sThumbRX, RIGHT_THUMB_DEADZONE);
	float rightY = normalizeStick(pad.sThumbRY, RIGHT_THUMB_DEADZONE);

	setKey(static_cast<KeyCode>(Key::RIGHT_STICK_LEFT), (rightX < 0.0f) ? -rightX : 0.0f);
	setKey(static_cast<KeyCode>(Key::RIGHT_STICK_RIGHT), (rightX > 0.0f) ? rightX : 0.0f);
	setKey(static_cast<KeyCode>(Key::RIGHT_STICK_UP), (rightY > 0.0f) ? rightY : 0.0f);
	setKey(static_cast<KeyCode>(Key::RIGHT_STICK_DOWN), (rightY < 0.0f) ? -rightY : 0.0f);

	endUpdate();
}

bool Gamepad::isConnected() const {
	return m_connected;
}