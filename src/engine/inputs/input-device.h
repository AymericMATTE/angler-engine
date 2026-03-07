#pragma once

#include <array>

using KeyCode = unsigned char;
using KeyValue = float;

struct Rect {
	int x, y;
	unsigned int width, height;
};

class IInputDevice {
public:
	virtual ~IInputDevice() = default;

	virtual void update(float _delta, Rect _viewport) = 0;

	// Get current held state (continuous while pressed)
	KeyValue getKey(KeyCode _keycode) const;

	// Get pressed state (true only on first frame of press)
	KeyValue getKeyDown(KeyCode _keycode) const;

	// Get released state (true only on first frame of release)
	KeyValue getKeyUp(KeyCode _keycode) const;

	bool hasActivity() const;

protected:
	void beginUpdate();
	void endUpdate();

	void setKey(KeyCode _code, KeyValue _value);
	void setKeyRaw(KeyCode _code, KeyValue _value);

protected:
	std::array<KeyValue, 256> m_last = {};
	std::array<KeyValue, 256> m_current = {};
	std::array<KeyValue, 256> m_final = {};
	bool m_activity = false;
};