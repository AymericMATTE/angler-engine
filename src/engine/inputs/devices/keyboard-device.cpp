#include "pch.h"
#include "keyboard-device.h"

#include <windows.h>
#ifdef DELETE
#undef DELETE
#endif

int toVirtualKey(KeyCode _keycode) {
    using K = Keyboard::Key;

    switch (static_cast<K>(_keycode)) {
        // Default
        case K::NONE:               return 0;

        // Letters
        case K::A:                  return 'A';
        case K::B:                  return 'B';
        case K::C:                  return 'C';
        case K::D:                  return 'D';
        case K::E:                  return 'E';
        case K::F:                  return 'F';
        case K::G:                  return 'G';
        case K::H:                  return 'H';
        case K::I:                  return 'I';
        case K::J:                  return 'J';
        case K::K:                  return 'K';
        case K::L:                  return 'L';
        case K::M:                  return 'M';
        case K::N:                  return 'N';
        case K::O:                  return 'O';
        case K::P:                  return 'P';
        case K::Q:                  return 'Q';
        case K::R:                  return 'R';
        case K::S:                  return 'S';
        case K::T:                  return 'T';
        case K::U:                  return 'U';
        case K::V:                  return 'V';
        case K::W:                  return 'W';
        case K::X:                  return 'X';
        case K::Y:                  return 'Y';
        case K::Z:                  return 'Z';

        // UpperRow Numbers
        case K::ALPHA_0:            return '0';
        case K::ALPHA_1:            return '1';
        case K::ALPHA_2:            return '2';
        case K::ALPHA_3:            return '3';
        case K::ALPHA_4:            return '4';
        case K::ALPHA_5:            return '5';
        case K::ALPHA_6:            return '6';
        case K::ALPHA_7:            return '7';
        case K::ALPHA_8:            return '8';
        case K::ALPHA_9:            return '9';

        // Keypad Numbers
        case K::KEYPAD_0:           return VK_NUMPAD0;
        case K::KEYPAD_1:           return VK_NUMPAD1;
        case K::KEYPAD_2:           return VK_NUMPAD2;
        case K::KEYPAD_3:           return VK_NUMPAD3;
        case K::KEYPAD_4:           return VK_NUMPAD4;
        case K::KEYPAD_5:           return VK_NUMPAD5;
        case K::KEYPAD_6:           return VK_NUMPAD6;
        case K::KEYPAD_7:           return VK_NUMPAD7;
        case K::KEYPAD_8:           return VK_NUMPAD8;
        case K::KEYPAD_9:           return VK_NUMPAD9;

        // Keypad Operators
        case K::KEYPAD_MULTIPLY:    return VK_MULTIPLY;
        case K::KEYPAD_ADD:         return VK_ADD;
        case K::KEYPAD_SEPARATOR:   return VK_SEPARATOR;
        case K::KEYPAD_SUBTRACT:    return VK_SUBTRACT;
        case K::KEYPAD_DECIMAL:     return VK_DECIMAL;
        case K::KEYPAD_DIVIDE:      return VK_DIVIDE;

        // Functions
        case K::F1:                 return VK_F1; 
        case K::F2:                 return VK_F2;
        case K::F3:                 return VK_F3; 
        case K::F4:                 return VK_F4;
        case K::F5:                 return VK_F5; 
        case K::F6:                 return VK_F6;
        case K::F7:                 return VK_F7; 
        case K::F8:                 return VK_F8;
        case K::F9:                 return VK_F9; 
        case K::F10:                return VK_F10;
        case K::F11:                return VK_F11;
        case K::F12:                return VK_F12;
        case K::F13:                return VK_F13;
        case K::F14:                return VK_F14;
        case K::F15:                return VK_F15;
        case K::F16:                return VK_F16;
        case K::F17:                return VK_F17;
        case K::F18:                return VK_F18;
        case K::F19:                return VK_F19;
        case K::F20:                return VK_F20;
        case K::F21:                return VK_F21;
        case K::F22:                return VK_F22;
        case K::F23:                return VK_F23;
        case K::F24:                return VK_F24;

        // Modifiers
        case K::LSHIFT:             return VK_LSHIFT;
        case K::RSHIFT:             return VK_RSHIFT;
        case K::LCONTROL:           return VK_LCONTROL;
        case K::RCONTROL:           return VK_RCONTROL;
        case K::LALT:               return VK_LMENU; 
        case K::RALT:               return VK_RMENU;
        case K::LWIN:               return VK_LWIN;  
        case K::RWIN:               return VK_RWIN;
        case K::MENU:               return VK_APPS;

        // Navigation / editing
        case K::ESCAPE:             return VK_ESCAPE;
        case K::TAB:                return VK_TAB;
        case K::CAPSLOCK:           return VK_CAPITAL;
        case K::BACKSPACE:          return VK_BACK;
        case K::ENTER:              return VK_RETURN;
        case K::SPACE:              return VK_SPACE;
        case K::INSERT:             return VK_INSERT;
        case K::DELETE:             return VK_DELETE;
        case K::HOME:               return VK_HOME;
        case K::END:                return VK_END;
        case K::PAGEUP:             return VK_PRIOR;
        case K::PAGEDOWN:           return VK_NEXT;
        case K::PRINTSCREEN:        return VK_SNAPSHOT;
        case K::PAUSE:              return VK_PAUSE;
        case K::SCROLLLOCK:         return VK_SCROLL;

        // Arrows
        case K::UP:                 return VK_UP;
        case K::DOWN:               return VK_DOWN;
        case K::LEFT:               return VK_LEFT;
        case K::RIGHT:              return VK_RIGHT;

        // Symbols / Punctuation
        case K::MINUS:              return VK_OEM_MINUS;
        case K::EQUAL:              return VK_OEM_PLUS;
        case K::LBRACKET:           return VK_OEM_4;
        case K::RBRACKET:           return VK_OEM_6;
        case K::BACKSLASH:          return VK_OEM_5;
        case K::SEMICOLON:          return VK_OEM_1;
        case K::APOSTROPHE:         return VK_OEM_7;
        case K::COMMA:              return VK_OEM_COMMA;
        case K::PERIOD:             return VK_OEM_PERIOD;
        case K::SLASH:              return VK_OEM_2;
        case K::TILDE:              return VK_OEM_3;

        default:                    return 0;
    }
}

void Keyboard::update(float _delta, Rect _viewport) {
    beginUpdate();

    for (KeyCode code = 0; code < 255; ++code) {
        int vk = toVirtualKey(code);
        if (vk == 0) continue;

        SHORT state = GetAsyncKeyState(vk);
        KeyValue value = (state & 0x8000) ? 1.0f : 0.0f;

        setKey(code, value);
    }

    endUpdate();
}