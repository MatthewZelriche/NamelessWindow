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
 * @brief Defines the state of a mouse button at the moment the event was generated.
 * @ingroup Common
 * @headerfile "Events/InputValues.hpp"
 */
enum class ButtonPressType { UNKNOWN = -1, PRESSED = 0, RELEASED = 1 };

/*!
 * @brief Defines the direction for a scroll event.
 * @ingroup Common
 * @headerfile "Events/InputValues.hpp"
 *
 * Scrolling is currently treated as a discrete button-like event, as opposed to an analog event. Mice that
 * provide "smooth/analog" scrolling are not currently supported.
 */
enum class ScrollType { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 };

/**
 * Defines the supported mouse buttons.
 * @todo Look into supporting a larger number of buttons.
 * @ingroup Common
 * @headerfile "Events/InputValues.hpp"
 */
enum class ButtonValue { NULLCLICK = -1, LEFTCLICK = 0, RIGHTCLICK = 1, MIDDLECLICK = 2, MB_4 = 3, MB_5 = 4 };

}  // namespace NLSWIN