#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>
#define explicit explicit_
#include <xcb/xkb.h>
#include <xkbcommon/xkbcommon-x11.h>
#undef explicit

#include <MagicEnum/magic_enum.hpp>
#include <array>
#include <unordered_map>

#include "EventListener.x11.hpp"
#include "InputDevice.x11.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/NLSAPI.h"

namespace NLSWIN {
class Window;
class NLSWIN_API_PRIVATE Keyboard::Impl : public InputDeviceX11 {
   private:
   static constexpr std::size_t m_NumKeys = magic_enum::enum_count<KeyValue>();
   std::array<bool, m_NumKeys> m_InternalKeyState;
   xkb_context *m_keyboardContext;
   xkb_state *m_KeyboardState;
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;
   void Init(xcb_input_device_id_t deviceID);

   public:
   Impl();
   Impl(KeyboardDeviceInfo device);
   [[nodiscard]] Event ProcessKeyEvent(xcb_ge_generic_event_t *event);
   [[nodiscard]] xkb_keysym_t GetSymFromKeyCode(unsigned int keycode);
   [[nodiscard]] KeyModifiers ParseModifierState(uint32_t mods);
};
}  // namespace NLSWIN