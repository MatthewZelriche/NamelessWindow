#pragma once

#include "../NLSAPI.h"

namespace NLSWIN {

enum class NLSWIN_API_PUBLIC KeyPressType { PRESSED = 0, RELEASED = 1, REPEAT = 2 };
enum class NLSWIN_API_PUBLIC ButtonPressType { PRESSED = 0, RELEASED = 1 };
enum class NLSWIN_API_PUBLIC ScrollType { UP = 0, DOWN, LEFT, RIGHT };

enum class NLSWIN_API_PUBLIC KeyModifierState {
   CTRL = 1 << 1,
   SUPER = 1 << 2,
   ALT = 1 << 3,
   SHIFT = 1 << 4,
   CAPSLCK = 1 << 5

};

enum class NLSWIN_API_PUBLIC ButtonValue { LEFTCLICK = 1, RIGHTCLICK, MIDDLECLICK, MB_4, MB_5, NULLCLICK };

// TODO: Support more keys.
enum class NLSWIN_API_PUBLIC KeyValue {
   KEY_NULL = -1,
   KEY_0,
   KEY_1,
   KEY_2,
   KEY_3,
   KEY_4,
   KEY_5,
   KEY_6,
   KEY_7,
   KEY_8,
   KEY_9,
   KEY_A,
   KEY_B,
   KEY_C,
   KEY_D,
   KEY_E,
   KEY_F,
   KEY_G,
   KEY_H,
   KEY_I,
   KEY_J,
   KEY_K,
   KEY_L,
   KEY_M,
   KEY_N,
   KEY_O,
   KEY_P,
   KEY_Q,
   KEY_R,
   KEY_S,
   KEY_T,
   KEY_U,
   KEY_V,
   KEY_W,
   KEY_X,
   KEY_Y,
   KEY_Z,
   KEY_ESC,
   KEY_LSHIFT,
   KEY_RSHIFT,
   KEY_LCTRL,
   KEY_RCTRL,
   KEY_LSUPER,
   KEY_RSUPER,
   KEY_LALT,
   KEY_RALT,
   KEY_SPACE,
   KEY_COMMA,
   KEY_PERIOD,
   KEY_FORWARDSLASH,
   KEY_SEMICOLON,
   KEY_APOSTROPHE,
   KEY_BACKSLASH,
   KEY_ENTER,
   KEY_LBRACKET,
   KEY_RBRACKET,
   KEY_DASH,
   KEY_EQUALS,
   KEY_BACKSPACE,
   KEY_TILDE,
   KEY_CAPSLOCK,
   KEY_NUMPAD_DIVIDE,
   KEY_NUMPAD_MULTIPLY,
   KEY_NUMPAD_SUBTRACT,
   KEY_NUMPAD_ADD,
   KEY_NUMPAD_ENTER
};

struct NLSWIN_API_PUBLIC KeyModifiers {
   bool ctrl : 1;
   bool super : 1;
   bool alt : 1;
   bool shift : 1;
   bool capslock : 1;
};

struct NLSWIN_API_PUBLIC KeyCode {
   KeyValue value;
   KeyPressType pressType;
   KeyModifiers modifiers;
};

}  // namespace NLSWIN