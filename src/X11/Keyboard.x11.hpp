#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

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
   // Private init method to avoid code duplication in overloaded constructors.
   void Init(const Window &window);

   public:
   Impl(const Window &window);
   Impl(const Window &window, KeyboardDeviceInfo device);
};

struct NLSWIN_API_PRIVATE XI2EventMask {
   xcb_input_event_mask_t head;
   xcb_input_xi_event_mask_t mask;
};
}  // namespace NLSWIN