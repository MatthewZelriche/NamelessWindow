#pragma once

#include <xcb/xcb.h>
#include <xcb/xfixes.h>
#include <xcb/xinput.h>

#include <MagicEnum/magic_enum.hpp>

#include "InputDevice.x11.hpp"
#include "NamelessWindow/NLSAPI.h"
#include "NamelessWindow/Pointer.hpp"

namespace NLSWIN {
class Window;
class NLSWIN_API_PRIVATE PointerDeviceX11 : public InputDeviceX11, public Pointer {
   private:
   void RequestShowCursor() override;
   void RequestHiddenCursor() override;
   bool m_shouldCursorBeHidden {false};
   xcb_cursor_t m_cursor {0};

   protected:
   xcb_window_t m_currentInhabitedWindow {0};
   xcb_window_t m_boundWindow {0};
   bool m_attemptGrabNextPoll {false};
   [[nodiscard]] inline bool ClientRequestedHiddenCursor() { return m_shouldCursorBeHidden; };

   void HideCursor();
   void ShowCursor();
   [[nodiscard]] bool AttemptCursorGrab(xcb_window_t window);
   void UngrabCursor();

   void PackageButtonPressEvent(xcb_input_button_press_event_t *event);
   void PackageButtonReleaseEvent(xcb_input_button_release_event_t *event);
   void PackageEnterEvent(xcb_input_enter_event_t *event);
   void PackageLeaveEvent(xcb_input_leave_event_t *event);
   void PackageMotionEvent(xcb_input_motion_event_t *event);
   void PackageDeltaEvent(xcb_input_raw_motion_event_t *event);

   float lastX {0};
   float lastY {0};
   inline float TranslateXCBFloat(xcb_input_fp1616_t inval) { return inval / (float)UINT16_MAX; }
   inline float TranslateXCBFloat(xcb_input_fp3232_t inval) {
      return inval.integral + inval.frac / (float)UINT32_MAX;
   }

   public:
   PointerDeviceX11(xcb_input_device_id_t deviceID);
};
}  // namespace NLSWIN