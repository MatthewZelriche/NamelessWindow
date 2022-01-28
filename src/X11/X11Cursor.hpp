#pragma once

#include <set>

#include "NamelessWindow/Cursor.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "NamelessWindow/Window.hpp"
#include "X11GenericMouse.hpp"
#include "X11InputDevice.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE X11Cursor : public X11GenericMouse, public X11InputDevice, virtual public Cursor {
   public:
   X11Cursor();
   void BindToWindow(const Window *const window) noexcept override;
   void ShowCursor() noexcept override;
   void HideCursor() noexcept override;

   protected:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;
   void AttemptSetVisible();
   void AttemptSetHidden();
   Event PackageNewButtonPressEvent(xcb_button_press_event_t *event, xcb_window_t sourceWindow);
   Event PackageNewButtonReleaseEvent(xcb_button_release_event_t *event, xcb_window_t sourceWindow);
   Event PackageNewMoveEvent(xcb_motion_notify_event_t *motionEvent, xcb_window_t sourceWindow);

   private:
   xcb_cursor_t m_cursor;
   bool m_isHidden {false};
   bool m_requestedHidden {false};
   float lastX {0};
   float lastY {0};
   xcb_window_t m_boundWindow;
   xcb_window_t m_inhabitedWindow;
   /*! @todo Will need to update this if we support multiple cursors. Right now it only expects one. */
   static xcb_input_device_id_t GetMasterPointerDeviceID() noexcept;
   bool WithinSubscribedWindow() const;
   const xcb_event_mask_t m_xcbEventMask {
      (xcb_event_mask_t)(XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                         XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_ENTER_WINDOW |
                         XCB_EVENT_MASK_LEAVE_WINDOW)};
   const xcb_input_xi_event_mask_t m_rawInputEventMask {
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_RAW_MOTION | XCB_INPUT_XI_EVENT_MASK_ENTER)};
};

}  // namespace NLSWIN