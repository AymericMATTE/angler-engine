#pragma once

#include "../input-device.h"

class Gamepad : public IInputDevice {
public:
	enum class Key : KeyCode {
		NONE,

		// Buttons
		A, B, X, Y,

		// Shoulders
		LEFT_SHOULDER, RIGHT_SHOULDER,

		// Triggers
		LEFT_TRIGGER_BTN, RIGHT_TRIGGER_BTN,

		// DPad
		DPAD_UP, DPAD_DOWN,
		DPAD_LEFT, DPAD_RIGHT,

		// Stick Buttons
		LEFT_THUMB, RIGHT_THUMB,

		// Utility Buttons
		START, BACK, GUIDE,

		// Left Stick
		LEFT_STICK_LEFT, LEFT_STICK_RIGHT,
		LEFT_STICK_UP, LEFT_STICK_DOWN,

		// Right Stick
		RIGHT_STICK_LEFT, RIGHT_STICK_RIGHT,
		RIGHT_STICK_UP, RIGHT_STICK_DOWN,

		// Triggers
		LEFT_TRIGGER_AXIS, RIGHT_TRIGGER_AXIS
	};

	virtual void update(float _dt, Rect _viewport) override;

	bool isConnected() const; // For later if I add multiple gamepad support

private:
	bool m_connected = false;

	float normalizeStick(short _value, float _deadzone);
};