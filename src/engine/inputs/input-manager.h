#pragma once

#include "input-device.h"
#include "devices/mouse-device.h"
#include "devices/gamepad-device.h"
#include "devices/keyboard-device.h"

#include <unordered_map>
#include <string>


struct Action {
	Keyboard::Key keyboardBinding = Keyboard::Key::NONE;

	Mouse::Key mouseBinding = Mouse::Key::NONE;

	Gamepad::Key gamepadBinding = Gamepad::Key::NONE;
};

struct Axis {
	Keyboard::Key keyboardPositiveBinding = Keyboard::Key::NONE;
	Keyboard::Key keyboardNegativeBinding = Keyboard::Key::NONE;

	Mouse::Key mousePositiveBinding = Mouse::Key::NONE;
	Mouse::Key mouseNegativeBinding = Mouse::Key::NONE;

	Gamepad::Key gamepadPositiveBinding = Gamepad::Key::NONE;
	Gamepad::Key gamepadNegativeBinding = Gamepad::Key::NONE;
};

class InputManager {
public:
	static void init();

	static void addAction(const char* _name, const Action& _action);
	static void addAxis(const char* _name, const Axis& _axis);

	static float getAxis(const char* _axis);

	static bool getAction(const char* _action);
	static bool getActionDown(const char* _action);
	static bool getActionUp(const char* _action);

	static void update(float _delta, Rect _viewport);

private:
	InputManager() = default;
	~InputManager() = default;
	
	static InputManager* m_instance;

	std::unordered_map<std::string, Action> m_actions;
	std::unordered_map<std::string, Axis> m_axes;

	// Devices
	Keyboard m_keyboard;
	Mouse m_mouse;
	Gamepad m_gamepad;
};