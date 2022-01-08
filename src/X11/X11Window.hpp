#pragma once

#include <xcb/xcb.h>

#include "NamelessWindow/Window.hpp"
#include "X11EventListener.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE X11Window : public Window, public X11EventListener {
   public:
   X11Window(WindowProperties properties);

   private:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;
   xcb_window_t m_x11WindowID {0};
   const xcb_event_mask_t m_eventMask =
      (xcb_event_mask_t)(XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_FOCUS_CHANGE |
                         XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_ENTER_WINDOW |
                         XCB_EVENT_MASK_VISIBILITY_CHANGE);
};
}  // namespace NLSWIN