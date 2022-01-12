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
#include "XConnection.h"

namespace NLSWIN {

/*! @ingroup X11 */
class NLSWIN_API_PRIVATE X11InputDevice : public X11EventListener, virtual public InputDevice {
   public:
   /*!
    * @brief Takes an Xinput2 event, and either constructs a platform-independent Event object to store in
    * its queue, or discards the event.
    *
    * @param event The XInput2 event to process.
    */
   virtual void ProcessXInputEvent(xcb_ge_generic_event_t *event) = 0;
   /*!
    * @brief Inform the X server that this deviceID wishes to receive XInput2 events from a specific window.
    *
    * @param x11Handle The window to receive events from.
    * @param windowID The NLSWIN unique identifier of the X11 window.
    */
   void SubscribeToWindow(const Window *const window) override;
   /*!
    * @brief Inform the X server that this deviceID no longer wishes to receive XInput2 events from a specific
    * window.
    *
    * @param x11Handle The window to no longer receive events from.
    * @param windowID The NLSWIN unique identifier of the X11 window.
    * @todo Test to confirm this function doesn't inadvertantly wipe the event mask for EVERY window.
    */
   void UnsubscribeFromWindow(const Window *const window) override;

   protected:
   const std::unordered_map<xcb_window_t, WindowID> &GetSubscribedWindows() const noexcept {
      return m_subscribedWindows;
   }
   xcb_input_device_id_t m_deviceID {0};

   private:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;
   std::unordered_map<xcb_window_t, WindowID> m_subscribedWindows;
};

template <typename T>
[[nodiscard]] NLSWIN_API_PUBLIC std::vector<T> EnumerateDevicesX11(xcb_input_device_type_t type) noexcept {
   std::vector<T> devices;

   xcb_input_xi_query_device_cookie_t queryCookie =
      xcb_input_xi_query_device(XConnection::GetConnection(), XCB_INPUT_DEVICE_ALL);
   xcb_input_xi_query_device_reply_t *reply =
      xcb_input_xi_query_device_reply(XConnection::GetConnection(), queryCookie, nullptr);

   xcb_input_xi_device_info_iterator_t iter = xcb_input_xi_query_device_infos_iterator(reply);
   while (iter.rem > 0) {
      auto element = iter.data;
      if (element->enabled) {
         if (type == XCB_INPUT_DEVICE_TYPE_SLAVE_KEYBOARD &&
             element->type == XCB_INPUT_DEVICE_TYPE_SLAVE_KEYBOARD) {
            // Ignore xtest devices
            const char *name = xcb_input_xi_device_info_name(element);
            if (!std::strstr(name, "XTEST")) {
               devices.push_back({name, element->deviceid});
            }
         } else if (type == XCB_INPUT_DEVICE_TYPE_SLAVE_POINTER &&
                    element->type == XCB_INPUT_DEVICE_TYPE_SLAVE_POINTER) {
            const char *name = xcb_input_xi_device_info_name(element);
            if (!std::strstr(name, "XTEST")) {
               devices.push_back({name, element->deviceid});
            }
         }
      }
      xcb_input_xi_device_info_next(&iter);
   }
   free(reply);
   return devices;
}

}  // namespace NLSWIN