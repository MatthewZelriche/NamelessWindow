#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <climits>
#include <unordered_map>
#define explicit explicit_
#include <xcb/xkb.h>
#include <xkbcommon/xkbcommon-x11.h>
#undef explicit
#include "NamelessWindow/MasterPointer.hpp"
#include "PointerDevice.x11.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE MasterPointer::Impl : public PointerDeviceX11 {
   private:
   xcb_input_device_id_t m_corePointerID {0};
   static xcb_input_device_id_t GetMasterPointerDeviceID();
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;
   bool m_disabled {false};

   public:
   Impl();

   inline void Disable() { m_disabled = true; }
   inline void Enable() { m_disabled = false; }
   [[nodiscard]] inline xcb_input_device_id_t GetMasterPointerID() const noexcept { return m_corePointerID; }
};
}  // namespace NLSWIN