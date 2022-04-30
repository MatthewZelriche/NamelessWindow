/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup X11 Linux X11 API
 * @brief Platform-specific X11 implementation of the API
 */
#pragma once

#include "NamelessWindow/Cursor.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "NamelessWindow/Window.hpp"
#include "X11GenericMouse.hpp"
#include "X11InputDevice.hpp"

namespace NLSWIN {

/*! @ingroup X11 */
class NLSWIN_API_PRIVATE X11Cursor : public X11GenericMouse, public X11InputDevice, virtual public Cursor {
   public:
   /*! @brief Construct a new X11Cursor object. */
   X11Cursor();
   void BindToWindow(const Window *const window) noexcept override;
   void Show() noexcept override;
   void Hide() noexcept override;
   void UnbindFromWindows() noexcept override;

   protected:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;
   /*! @brief Attempts to inform the X server to make the cursor icon visible when the cursor is within bounds
    * of a subscribed window, if certain conditions are met.
    */
   void AttemptSetVisible();
   /*! @brief Attempts to inform the X server to make the cursor icon invisible when the cursor is within
    * bounds of a subscribed window, if certain conditions are met.
    */
   void AttemptSetHidden();
   Event PackageNewButtonPressEvent(xcb_button_press_event_t *event, xcb_window_t sourceWindow);
   Event PackageNewButtonReleaseEvent(xcb_button_release_event_t *event, xcb_window_t sourceWindow);
   Event PackageNewMoveEvent(xcb_motion_notify_event_t *motionEvent, xcb_window_t sourceWindow);

   private:
   xcb_cursor_t m_cursor;
   /*! Is the cursor icon actually currently hidden? */
   bool m_isHidden {false};
   /*! Has the application requested we hide the cursor within subscribed windows? */
   bool m_requestedHidden {false};
   float lastX {0};
   float lastY {0};
   /*! The X11 window that the cursor is currently bound to, or 0. */
   xcb_window_t m_boundWindow;
   bool m_isTempUnbound {false};
   /*! The X11 window the cursor is currently inside. Value is 0 when not within a window that has been
    * subscribed to. */
   xcb_window_t m_inhabitedWindow;
   /*! @todo Will need to update this if we support multiple cursors. Right now it only expects one. */
   static xcb_input_device_id_t GetMasterPointerDeviceID() noexcept;
   const xcb_event_mask_t m_xcbEventMask {
      (xcb_event_mask_t)(XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                         XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_ENTER_WINDOW |
                         XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_FOCUS_CHANGE)};
   const xcb_input_xi_event_mask_t m_rawInputEventMask {
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_RAW_MOTION | XCB_INPUT_XI_EVENT_MASK_ENTER)};
};

}  // namespace NLSWIN