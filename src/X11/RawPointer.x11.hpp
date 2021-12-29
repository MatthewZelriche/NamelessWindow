#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <MagicEnum/magic_enum.hpp>

#include "InputDevice.x11.hpp"
#include "NamelessWindow/NLSAPI.h"
#include "NamelessWindow/RawPointer.hpp"
#include "PointerDevice.x11.hpp"

namespace NLSWIN {
class Window;
class NLSWIN_API_PRIVATE RawPointer::Impl : public PointerDeviceX11 {
   private:
   WindowID m_boundWindow {0};
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;

   public:
   Impl(xcb_input_device_id_t deviceID, const Window &window);
   void BindToWindow(const Window &window);
};
}  // namespace NLSWIN