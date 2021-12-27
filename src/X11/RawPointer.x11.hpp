#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <MagicEnum/magic_enum.hpp>

#include "InputDevice.x11.hpp"
#include "NamelessWindow/NLSAPI.h"
#include "NamelessWindow/RawPointer.hpp"

namespace NLSWIN {
class Window;
class NLSWIN_API_PRIVATE RawPointer::Impl : public InputDeviceX11 {
   private:
   static constexpr std::size_t m_NumKeys = magic_enum::enum_count<KeyValue>();
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;

   public:
   Impl(PointerDeviceInfo device);
   Impl(xcb_input_device_id_t deviceID);
};
}  // namespace NLSWIN