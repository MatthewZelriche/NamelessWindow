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
#include "NamelessWindow/Pointer.hpp"
#include "PointerDevice.x11.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE MasterPointerX11 : public PointerDeviceX11 {
   private:
   static bool m_instantiated;
   xcb_input_device_id_t m_corePointerID {0};
   static xcb_input_device_id_t GetMasterPointerDeviceID();
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;
   bool m_disabled {false};

   void BindToWindow(const Window *const window) override;
   void UnbindFromWindow() override;

   public:
   MasterPointerX11();
   void OnFocusOut(xcb_focus_out_event_t *event);

   [[nodiscard]] inline xcb_input_device_id_t GetMasterPointerID() const noexcept { return m_corePointerID; }
};
}  // namespace NLSWIN