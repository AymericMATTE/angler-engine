#pragma once

#include "../input-device.h"

// Undef win macros
#ifdef INSERT
#undef INSERT
#endif

#ifdef DELETE
#undef DELETE
#endif

class Keyboard : public IInputDevice {
public:
	enum class Key : KeyCode {
		// Default
		NONE,

		// Letters
		A, B, C, D, E, F, G,
		H, I, J, K, L, M, N,
		O, P, Q, R, S, T, U,
		V, W, X, Y, Z,

		// UpperRow Numbers
		ALPHA_0, ALPHA_1, ALPHA_2, ALPHA_3,
		ALPHA_4, ALPHA_5, ALPHA_6, ALPHA_7,
		ALPHA_8, ALPHA_9,

		// Keypad Numbers
		KEYPAD_0, KEYPAD_1, KEYPAD_2, KEYPAD_3,
		KEYPAD_4, KEYPAD_5, KEYPAD_6, KEYPAD_7,
		KEYPAD_8, KEYPAD_9,

		// Keypad Operators
		KEYPAD_MULTIPLY, KEYPAD_ADD, KEYPAD_SEPARATOR,
		KEYPAD_SUBTRACT, KEYPAD_DECIMAL, KEYPAD_DIVIDE,

		// Functions
		F1, F2, F3, F4, F5, F6,
		F7, F8, F9, F10, F11, F12,
		F13, F14, F15, F16, F17, F18,
		F19, F20, F21, F22, F23, F24,

		// Modifiers
		LSHIFT, RSHIFT,
		LCONTROL, RCONTROL,
		LALT, RALT,
		LWIN, RWIN,
		MENU,

		// Navigation
		ESCAPE, TAB, CAPSLOCK, BACKSPACE, ENTER, SPACE,
		INSERT, DELETE, HOME, END, PAGEUP, PAGEDOWN,
		PRINTSCREEN, PAUSE, SCROLLLOCK,

		// Arrows
		UP, DOWN, LEFT, RIGHT,

		// Symbols / Punctuation
		MINUS, EQUAL, LBRACKET, RBRACKET, BACKSLASH,
		SEMICOLON, APOSTROPHE, COMMA, PERIOD, SLASH, TILDE
	};

	virtual void update(float _delta, Rect _viewport) override;
};