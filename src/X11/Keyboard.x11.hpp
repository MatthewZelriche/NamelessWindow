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
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/NLSAPI.h"

namespace NLSWIN {
class Window;
class NLSWIN_API_PRIVATE Keyboard::Impl : public EventListenerX11 {
   private:
   static xcb_connection_t *m_connection;
   const xcb_input_xi_event_mask_t m_subscribedMasks {
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_KEY_PRESS | XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE)};
   xcb_input_device_id_t m_deviceID;
   static constexpr std::size_t m_NumKeys = magic_enum::enum_count<KeyValue>();
   std::array<bool, m_NumKeys> m_InternalKeyState;
   xkb_context *m_keyboardContext;
   xkb_state *m_KeyboardState;
   std::unordered_map<xcb_window_t, WindowID> m_SubscribedWindows;
   void ProcessGenericEvent(xcb_generic_event_t *event) override;

   public:
   Impl();
   Impl(KeyboardDeviceInfo device);
   void SubscribeToWindow(const Window &window);
   [[nodiscard]] Event ProcessKeyEvent(xcb_ge_generic_event_t *event);
   [[nodiscard]] xkb_keysym_t GetSymFromKeyCode(unsigned int keycode);
   [[nodiscard]] KeyModifiers ParseModifierState(uint32_t mods);

   [[nodiscard]] xcb_input_device_id_t GetDeviceID() { return m_deviceID; }
};

struct NLSWIN_API_PRIVATE XI2EventMask {
   xcb_input_event_mask_t head;
   xcb_input_xi_event_mask_t mask;
};
}  // namespace NLSWIN