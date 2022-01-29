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
    * @brief Inform the X server that this deviceID wishes to receive input events from a specific window.
    *
    * @param x11Handle The window to receive events from.
    * @param windowID The NLSWIN unique identifier of the X11 window.
    */
   void SubscribeToWindow(const std::weak_ptr<Window> window) override;
   /*!
    * @brief Inform the X server that this deviceID no longer wishes to receive input events from a specific
    * window.
    *
    * @param x11Handle The window to no longer receive events from.
    * @param windowID The NLSWIN unique identifier of the X11 window.
    */
   void UnsubscribeFromWindow(const std::weak_ptr<Window> window) override;
   /*! @brief Inform the X server that you wish to receive Xinput2 events from specific windows you subscribe
    * to.
    *
    * Not necessary if you only need to subscribe to regular XCB events or raw events from the root
    * window. */
   void SubscribeToWindowSpecificXInput2Events(xcb_input_xi_event_mask_t eventMask);

   protected:
   const std::unordered_map<xcb_window_t, std::weak_ptr<X11Window>> &GetSubscribedWindows() const noexcept {
      return m_subscribedWindows;
   }

   private:
   xcb_input_xi_event_mask_t m_windowSpecificXInput2SubscribedEvents {(xcb_input_xi_event_mask_t)0};
   std::unordered_map<xcb_window_t, std::weak_ptr<X11Window>> m_subscribedWindows;
};

}  // namespace NLSWIN