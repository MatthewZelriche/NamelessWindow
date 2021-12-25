#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <unordered_map>
#define explicit explicit_
#include <xcb/xkb.h>
#include <xkbcommon/xkbcommon-x11.h>
#undef explicit

#include "EventListener.x11.hpp"
#include "NamelessWindow/Pointer.hpp"

namespace NLSWIN {
class Window;
class NLSWIN_API_PRIVATE Pointer::Impl : public EventListenerX11 {
   private:
   xcb_connection_t *m_connection;
   xkb_context *m_context;

   const xcb_input_xi_event_mask_t m_subscribedMasks {
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS |
                                  XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE | XCB_INPUT_XI_EVENT_MASK_MOTION)};
   xcb_input_device_id_t m_deviceID;
   std::unordered_map<xcb_window_t, WindowID> m_SubscribedWindows;
   void ProcessGenericEvent(xcb_generic_event_t *event) override;
   void Init(xcb_input_device_id_t deviceID);

   inline float TranslateXCBFloat(xcb_input_fp1616_t inval) { return inval / (float)UINT16_MAX; }

   public:
   Impl();
   Impl(PointerDeviceInfo device);
   void SubscribeToWindow(const Window &window);
   [[nodiscard]] Event ProcessKeyEvent(xcb_ge_generic_event_t *event);
   [[nodiscard]] xcb_input_device_id_t GetDeviceID() { return m_deviceID; }
};
}  // namespace NLSWIN