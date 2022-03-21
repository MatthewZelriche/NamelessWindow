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

}  // namespace NLSWIN