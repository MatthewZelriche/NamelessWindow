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

#include <cstring>

#include "NamelessWindow/InputDevice.hpp"
#include "X11EventListener.hpp"
#include "XConnection.h"

namespace NLSWIN {

/*! @ingroup X11 */
class NLSWIN_API_PRIVATE X11RawInputDevice : public X11EventListener {
   public:
   void SubscribeToRawRootEvents(xcb_input_xi_event_mask_t masks);

   protected:
   xcb_input_device_id_t m_deviceID {0};
};

/*! @ingroup X11 */
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
            // TODO: Proper handling of "Fake" keyboards such as power buttons.
            const char *name = xcb_input_xi_device_info_name(element);
            if (!std::strstr(name, "XTEST") && !std::strstr(name, "Power Button")) {
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