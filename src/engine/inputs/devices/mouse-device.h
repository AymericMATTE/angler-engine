#pragma once

#include "../input-device.h"

class Mouse : public IInputDevice {
public:
	enum class Key : KeyCode {
		NONE,

		// Buttons
		BUTTON_LEFT, BUTTON_RIGHT, BUTTON_MIDDLE,
		BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8,

		// Movement Delta
		DELTA_UP, DELTA_DOWN, DELTA_LEFT, DELTA_RIGHT,

		// Scroll Wheel
		SCROLL_UP, SCROLL_DOWN
	};

	static void lock();
	static void unlock();

	virtual void update(float _delta, Rect _viewport) override;
	void getPosition(float* _x, float* _y);

private:
	static bool m_locked; // TO DO : make it non-static
	static bool m_wasLocked; // TO DO : make it non-static / remove
	
	float m_lastX = 0.0f;
	float m_lastY = 0.0f;
	int m_accumulatedScroll = 0;
};