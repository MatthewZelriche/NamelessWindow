#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <climits>
#include <unordered_map>
#define explicit explicit_
#include <xcb/xkb.h>
#include <xkbcommon/xkbcommon-x11.h>
#undef explicit

#include "EventListener.x11.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE PointerX11 {
   private:
   inline float TranslateXCBFloat(xcb_input_fp1616_t inval) { return inval / (float)UINT16_MAX; }
   inline float TranslateXCBFloat(xcb_input_fp3232_t inval) {
      return inval.integral + inval.frac / (float)UINT32_MAX;
   }
   xcb_window_t m_currentInhabitedWindow {0};
   xcb_input_device_id_t m_corePointerID {0};
   xcb_connection_t *m_connection {nullptr};
   static xcb_input_device_id_t GetMasterPointerDeviceID(xcb_connection_t *connection);

   float lastX {0};
   float lastY {0};

   public:
   PointerX11();
   void ProcessXInputEvent(xcb_ge_generic_event_t *event, xcb_window_t x11WindowID,
                           EventListenerX11 *listener);

   [[nodiscard]] inline xcb_input_device_id_t GetMasterPointerID() const noexcept { return m_corePointerID; }
};
}  // namespace NLSWIN