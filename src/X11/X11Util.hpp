/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup X11 Linux X11 API
 * @brief Platform-specific X11 implementation of the API
 */
#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN::UTIL {
/*! @ingroup X11 */
struct NLSWIN_API_PRIVATE XI2EventMask {
   xcb_input_event_mask_t header;
   xcb_input_xi_event_mask_t mask;
};

/**
 * @brief Gets the default X screen.
 * @ingroup X11
 * @return The default X screen.
 */
xcb_screen_t *GetDefaultScreen();

/**
 * @brief Gets the default X screen number.
 * @ingroup X11
 * @return The default X screen number.
 */
int GetDefaultScreenNumber();

/**
 * @brief Gets the root window of the default screen.
 * @ingroup X11
 * @return The root window.
 */
xcb_window_t GetRootWindow();

}  // namespace NLSWIN::UTIL