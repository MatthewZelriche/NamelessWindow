#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <cstring>
#include <unordered_map>

#include "EventListener.x11.hpp"
#include "NamelessWindow/NLSAPI.h"
#include "XConnection.h"

namespace NLSWIN {
class Window;
struct NLSWIN_API_PRIVATE XI2EventMask {
   xcb_input_event_mask_t head;
   xcb_input_xi_event_mask_t mask;
};

class NLSWIN_API_PRIVATE InputDeviceX11 : public EventListenerX11 {
   private:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;

   protected:
   std::unordered_map<xcb_window_t, WindowID> m_SubscribedWindows;
   xcb_input_device_id_t m_deviceID;
   xcb_connection_t *m_connection;

   virtual void ProcessXInputEvent(xcb_ge_generic_event_t *event) = 0;

   public:
   [[nodiscard]] xcb_input_device_id_t GetDeviceID() { return m_deviceID; }
   void SubscribeToWindow(xcb_window_t x11Handle, WindowID windowID, xcb_input_xi_event_mask_t masks);
};

template <typename T>
[[nodiscard]] NLSWIN_API_PUBLIC std::vector<T> EnumerateDevicesX11(xcb_input_device_type_t type) noexcept {
   XConnection::CreateConnection();
   xcb_connection_t *connection = XConnection::GetConnection();
   std::vector<T> devices;

   xcb_input_xi_query_device_cookie_t queryCookie =
      xcb_input_xi_query_device(connection, XCB_INPUT_DEVICE_ALL);
   xcb_input_xi_query_device_reply_t *reply =
      xcb_input_xi_query_device_reply(connection, queryCookie, nullptr);

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