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

#include <xcb/xinput.h>

#include <cassert>
#include <cstring>
#include <unordered_map>

#include "NamelessWindow/InputDevice.hpp"
#include "NamelessWindow/Window.hpp"
#include "X11EventListener.hpp"
#include "X11RawInputDevice.hpp"
#include "X11Window.hpp"
#include "XConnection.h"

namespace NLSWIN {

/*! @ingroup X11 */
class NLSWIN_API_PRIVATE X11InputDevice : public X11RawInputDevice, virtual public InputDevice {
   public:
   /*!
    * @brief Inform the X server that this deviceID wishes to receive XInput2 events from a specific window.
    *
    * @param x11Handle The window to receive events from.
    * @param windowID The NLSWIN unique identifier of the X11 window.
    */
   void SubscribeToWindow(const std::weak_ptr<Window> window) override;
   /*!
    * @brief Inform the X server that this deviceID no longer wishes to receive XInput2 events from a specific
    * window.
    *
    * @param x11Handle The window to no longer receive events from.
    * @param windowID The NLSWIN unique identifier of the X11 window.
    * @todo Test to confirm this function doesn't inadvertantly wipe the event mask for EVERY window.
    */
   void UnsubscribeFromWindow(const std::weak_ptr<Window> window) override;

   protected:
   const std::unordered_map<xcb_window_t, std::weak_ptr<X11Window>> &GetSubscribedWindows() const noexcept {
      return m_subscribedWindows;
   }

   private:
   std::unordered_map<xcb_window_t, std::weak_ptr<X11Window>> m_subscribedWindows;
};

}  // namespace NLSWIN