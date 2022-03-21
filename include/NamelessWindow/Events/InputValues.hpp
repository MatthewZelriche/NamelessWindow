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
enum KeyValue: int;
}  // namespace NLSWIN