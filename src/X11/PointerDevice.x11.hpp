#pragma once

#include <xcb/xcb.h>

#include <climits>

#include "InputDevice.x11.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "NamelessWindow/Pointer.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE PointerDeviceX11 : public InputDeviceX11, virtual public Pointer {
   private:
   bool m_grabOnNextView {false};
   xcb_cursor_t m_cursor {0};

   protected:
   xcb_window_t m_currentInhabitedWindow {0};
   xcb_window_t m_boundWindow {0};
   float lastX {0};
   float lastY {0};
   void AttemptCursorGrab(xcb_window_t window);
   void UngrabCursor();

   void PackageButtonPressEvent(xcb_input_button_press_event_t *event);
   void PackageButtonReleaseEvent(xcb_input_button_release_event_t *event);
   void PackageEnterEvent(xcb_input_enter_event_t *event);
   void PackageLeaveEvent(xcb_input_leave_event_t *event);
   void PackageMotionEvent(xcb_input_motion_event_t *event);
   void PackageDeltaEvent(xcb_input_raw_motion_event_t *event);

   inline float TranslateXCBFloat(xcb_input_fp1616_t inval) { return inval / (float)UINT16_MAX; }
   inline float TranslateXCBFloat(xcb_input_fp3232_t inval) {
      return inval.integral + inval.frac / (float)UINT32_MAX;
   }

   public:
   PointerDeviceX11(xcb_input_device_id_t deviceID);
   inline bool shouldGrabOnNextVisibilityEvent() { return m_grabOnNextView; }
   inline xcb_window_t BoundWindow() { return m_boundWindow; }

   ~PointerDeviceX11() = default;
};
}  // namespace NLSWIN