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
#include <string>
#include <X11/extensions/Xrandr.h>

#include "NamelessWindow/NLSAPI.hpp"
#include "NamelessWindow/Window.hpp"

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

/**
 * @brief Gets the monitor info struct whose area the top-right corner of a given window falls within.
 * If the given window does not fall within any monitor area, this function will return the primary monitor.
 * @ingroup X11
 * @param topRightWindowPos The x,y coordinate of the top-right corner of the window.
 * @param window The X11 ID for the window.
 * @return X11 Info struct about the monitor. 
 */
XRRMonitorInfo *GetCurrentMonitor(xcb_window_t window, Point topRightWindowPos);

/**
 * @brief Determines if an x,y point lies within a rectangle.
 * @ingroup X11
 * @param rectangle The area to check.
 * @param position The position to look for within rectangle.
 * @return True if the point falls within the rectangle.
 */
bool IsPointInRect(Rect rectangle, Point position);

}  // namespace NLSWIN::UTIL