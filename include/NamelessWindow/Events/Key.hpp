/*!
 * @file Events/Key.hpp
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup Common Public API
 * @brief Documentation for public API that clients directly interact with.
 */
#pragma once
#include "../NLSAPI.hpp"
#include "InputValues.hpp"

namespace NLSWIN {

/*!
 * @brief Defines the state of a key at the moment the KeyEvent was generated.
 * @ingroup Common
 * @headerfile "Events/Key.hpp"
 */
enum class KeyPressType { UNKNOWN = -1, PRESSED = 0, RELEASED = 1, REPEAT = 2 };

/*!
 * @brief Defines the state of a pointer button at the moment the event was generated.
 * @ingroup Common
 * @headerfile "Events/Key.hpp"
 */
enum class ButtonPressType { PRESSED = 0, RELEASED = 1 };

/*!
 * Defines which modifier keys were active during a KeyEvent
 * @ingroup Common
 * @headerfile "Events/Key.hpp"
 */
struct NLSWIN_API_PUBLIC KeyModifiers {
   bool ctrl : 1;
   bool super : 1; /*! Also known as the Meta or Windows key. */
   bool alt : 1;
   bool shift : 1;
   bool capsLock : 1;
   bool scrollLock : 1;
   bool numLock : 1;
};

/*!
 * Represents the physical state of the keyboard at the time of the key event.
 * @ingroup Common
 * @headerfile "Events/Key.hpp"
 */
struct NLSWIN_API_PUBLIC KeyCode {
   KeyValue value {(KeyValue)-1};         /*! The keyboard key that resulted in this event. */
   KeyModifiers modifiers {false}; /*! State of modifier keys at time of event generation */
};


enum KeyValue: int {
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
   KEY_TAB,
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
   KEY_F1, 
   KEY_F2, 
   KEY_F3, 
   KEY_F4, 
   KEY_F5, 
   KEY_F6, 
   KEY_F7, 
   KEY_F8, 
   KEY_F9, 
   KEY_F10,
   KEY_F11,
   KEY_F12,
   KEY_LEFT,
   KEY_RIGHT,
   KEY_UP,
   KEY_DOWN,
   KEY_CAPSLOCK,
   KEY_SCROLL_LOCK,
   KEY_NUMLOCK,
   KEY_PRINTSCREEN,
   KEY_PAUSE,
   KEY_INSERT,
   KEY_HOME,
   KEY_PAGEUP,
   KEY_PAGEDOWN,
   KEY_DELETE,
   KEY_END,
   KEY_CLEAR,
   KEY_NUMPAD_0,
   KEY_NUMPAD_1,
   KEY_NUMPAD_2,
   KEY_NUMPAD_3,
   KEY_NUMPAD_4,
   KEY_NUMPAD_5,
   KEY_NUMPAD_6,
   KEY_NUMPAD_7,
   KEY_NUMPAD_8,
   KEY_NUMPAD_9,
   KEY_NUMPAD_PERIOD,
   KEY_NUMPAD_DIVIDE,
   KEY_NUMPAD_MULTIPLY,
   KEY_NUMPAD_SUBTRACT,
   KEY_NUMPAD_ADD,
   KEY_NUMPAD_ENTER
};

}  // namespace NLSWIN