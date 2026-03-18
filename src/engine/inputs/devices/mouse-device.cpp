#include "pch.h"
#include "mouse-device.h"

#include <windows.h>
#include <iostream>

bool Mouse::m_locked = false;
bool Mouse::m_wasLocked = false;

void Mouse::lock() {
	m_locked = true;
	m_wasLocked = false;
	ShowCursor(false);
}

void Mouse::unlock() {
	m_locked = false;
	ShowCursor(true);
	ClipCursor(nullptr);
}

void Mouse::update(float _delta, Rect _viewport) {
	beginUpdate();

	// Mouse Buttons
	SHORT leftState = GetAsyncKeyState(VK_LBUTTON);

	setKey(static_cast<KeyCode>(Key::BUTTON_LEFT), (leftState & 0x8000) ? 1.0f : 0.0f);

	SHORT rightState = GetAsyncKeyState(VK_RBUTTON);
	setKey(static_cast<KeyCode>(Key::BUTTON_RIGHT), (rightState & 0x8000) ? 1.0f : 0.0f);

	SHORT middleState = GetAsyncKeyState(VK_MBUTTON);
	setKey(static_cast<KeyCode>(Key::BUTTON_MIDDLE), (middleState & 0x8000) ? 1.0f : 0.0f);

	
	SHORT x1State = GetAsyncKeyState(VK_XBUTTON1);
	setKey(static_cast<KeyCode>(Key::BUTTON_3), (x1State & 0x8000) ? 1.0f : 0.0f);

	SHORT x2State = GetAsyncKeyState(VK_XBUTTON2);
	setKey(static_cast<KeyCode>(Key::BUTTON_4), (x2State & 0x8000) ? 1.0f : 0.0f);
	
	float viewportCenterX = _viewport.x + (_viewport.width * 0.5f);
	float viewportCenterY = _viewport.y + (_viewport.height * 0.5f);
	
	// Mouse Delta
	POINT currentPos;
	GetCursorPos(&currentPos);

	float currentX = static_cast<float>(currentPos.x);
	float currentY = static_cast<float>(currentPos.y);

	static bool first = true;
	if (first) {
		m_lastX = currentX;
		m_lastY = currentY;
		first = false;
	}

	float originX = m_lastX;
	float originY = m_lastY;

	if (m_locked) {
		originX = viewportCenterX;
		originY = viewportCenterY;
	}

	float deltaX = (currentPos.x - originX) / _delta * 0.016f;
	float deltaY = (currentPos.y - originY) / _delta * 0.016f;

	float senvity = 0.05f; // TO DO : Make it modifiable, remove it from here
	
	// First frame when I lock the cursor
	if (!m_wasLocked && m_locked) {
		RECT windowRect;
		windowRect.left = _viewport.x; windowRect.top = _viewport.y;
		windowRect.right = _viewport.x + _viewport.width; windowRect.bottom = _viewport.y + _viewport.height;
		ClipCursor(&windowRect);
		
		deltaX = 0.0f;
		deltaY = 0.0f;
		m_wasLocked = true;
	}

	setKey(static_cast<KeyCode>(Key::DELTA_RIGHT),	(deltaX > 0.0f) ? deltaX  * senvity : 0.0f);
	setKey(static_cast<KeyCode>(Key::DELTA_LEFT),	(deltaX < 0.0f) ? -deltaX * senvity : 0.0f);
	setKey(static_cast<KeyCode>(Key::DELTA_UP),		(deltaY < 0.0f) ? -deltaY * senvity : 0.0f);
	setKey(static_cast<KeyCode>(Key::DELTA_DOWN),	(deltaY > 0.0f) ? deltaY  * senvity : 0.0f);

	m_lastX = currentX;
	m_lastY = currentY;

	if (m_locked) {
		SetCursorPos(static_cast<int>(viewportCenterX), static_cast<int>(viewportCenterY));
	}

	// Update Scroll Wheel
	// TO DO : accumulate from WndProc WM_MOUSEWHEEL messages
	if (m_accumulatedScroll > 0) {
		setKey(static_cast<KeyCode>(Key::SCROLL_UP), 1.0f);
		setKey(static_cast<KeyCode>(Key::SCROLL_DOWN), 0.0f);
	}
	else if (m_accumulatedScroll < 0) {
		setKey(static_cast<KeyCode>(Key::SCROLL_UP), 0.0f);
		setKey(static_cast<KeyCode>(Key::SCROLL_DOWN), 1.0f);
	}
	else {
		setKey(static_cast<KeyCode>(Key::SCROLL_UP), 0.0f);
		setKey(static_cast<KeyCode>(Key::SCROLL_DOWN), 0.0f);
	}
	m_accumulatedScroll = 0;

	endUpdate();
}

void Mouse::getPosition(float* _x, float* _y)
{
	*_x = m_lastX;
	*_y = m_lastY;
}
