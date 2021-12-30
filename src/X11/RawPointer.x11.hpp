#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <MagicEnum/magic_enum.hpp>

#include "InputDevice.x11.hpp"
#include "NamelessWindow/NLSAPI.h"
#include "NamelessWindow/Pointer.hpp"
#include "PointerDevice.x11.hpp"

namespace NLSWIN {
class Window;
class NLSWIN_API_PRIVATE RawPointerX11 : public PointerDeviceX11 {
   private:
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;

   void BindToWindow(const Window *const window) override;
   void UnbindFromWindow() override;

   public:
   RawPointerX11(xcb_input_device_id_t deviceID, const Window *const window);
};
}  // namespace NLSWIN