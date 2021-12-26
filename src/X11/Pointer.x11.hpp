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
#include "InputDevice.x11.hpp"
#include "NamelessWindow/Pointer.hpp"

namespace NLSWIN {
class Window;
class NLSWIN_API_PRIVATE Pointer::Impl : public InputDeviceX11 {
   private:
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;
   inline float TranslateXCBFloat(xcb_input_fp1616_t inval) { return inval / (float)UINT16_MAX; }
   inline float TranslateXCBFloat(xcb_input_fp3232_t inval) {
      return inval.integral + inval.frac / (float)UINT32_MAX;
   }
   xcb_window_t m_currentInhabitedWindow {0};

   public:
   Impl();
   Impl(PointerDeviceInfo device);
   Impl(xcb_input_device_id_t deviceID);
};
}  // namespace NLSWIN