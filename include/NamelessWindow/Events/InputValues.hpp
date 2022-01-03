/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup Common Public API
 * @brief Documentation for public API that clients directly interact with.
 */
#pragma once
#include "../NLSAPI.hpp"

namespace NLSWIN {

/*!
 * @brief Defines the direction for a scroll event. Note that scrolling is treated as discrete instead of
 * smooth.
 * @ingroup Common
 * @headerfile "Events/InputValues.hpp"
 */
enum class ScrollType { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 };

/**
 * Defines the supported pointer buttons.
 * @todo Look into supporting a larger number of buttons - currently, Xinput2 on Linux seems to only support
 * up to five.
 * @ingroup Common
 * @headerfile "Events/InputValues.hpp"
 */
enum class ButtonValue { NULLCLICK = -1, LEFTCLICK = 0, RIGHTCLICK = 1, MIDDLECLICK = 2, MB_4 = 3, MB_5 = 4 };

/**
 * Defines the supported keyboard keys.
 * @todo Define more keys, not all keys on a standard keyboard are currently included.
 * @ingroup Common
 * @headerfile "Events/InputValues.hpp"
 */
enum class KeyValue {
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

}  // namespace NLSWIN