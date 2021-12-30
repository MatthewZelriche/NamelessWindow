#include "MasterPointer.x11.hpp"

#include <cstring>

#include "EventQueue.x11.hpp"
#include "InputDevice.x11.hpp"
#include "InputMapper.x11.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "Window.x11.hpp"
#include "X11/extensions/XInput2.h"
#include "XConnection.h"

using namespace NLSWIN;

bool MasterPointerX11::m_instantiated {false};

void MasterPointerX11::ProcessXInputEvent(xcb_ge_generic_event_t *event) {
   if (m_disabled) {
      return;
   }
   // Keep trying until we succeed at a grab.
   if (m_attemptGrabNextPoll) {
      if (AttemptCursorGrab(m_boundWindow)) {
         m_attemptGrabNextPoll = false;
      }
   }
   switch (event->event_type) {
      case XCB_INPUT_BUTTON_PRESS: {
         xcb_input_button_press_event_t *buttonPressEvent =
            reinterpret_cast<xcb_input_button_press_event_t *>(event);
         if (!(m_boundWindow && m_boundWindow != buttonPressEvent->event)) {
            PackageButtonPressEvent(buttonPressEvent);
         }
         break;
      }
      case XCB_INPUT_BUTTON_RELEASE: {
         xcb_input_button_release_event_t *buttonReleaseEvent =
            reinterpret_cast<xcb_input_button_release_event_t *>(event);
         if (!(m_boundWindow && m_boundWindow != buttonReleaseEvent->event)) {
            PackageButtonReleaseEvent(buttonReleaseEvent);
         }
         break;
      }
      case XCB_INPUT_ENTER: {
         xcb_input_enter_event_t *enterEvent = reinterpret_cast<xcb_input_enter_event_t *>(event);
         if (!(m_boundWindow && m_boundWindow != enterEvent->event)) {
            PackageEnterEvent(enterEvent);
         }
         if (ClientRequestedHiddenCursor()) {
            HideCursor();
         }
         break;
      }
      case XCB_INPUT_LEAVE: {
         xcb_input_leave_event_t *leaveEvent = reinterpret_cast<xcb_input_leave_event_t *>(event);
         if (!(m_boundWindow && m_boundWindow != leaveEvent->event)) {
            PackageLeaveEvent(leaveEvent);
         }
         ShowCursor();
         break;
      }
      case XCB_INPUT_MOTION: {
         xcb_input_motion_event_t *motionEvent = reinterpret_cast<xcb_input_motion_event_t *>(event);
         if (!(m_boundWindow && m_boundWindow != motionEvent->event)) {
            PackageMotionEvent(motionEvent);
         }
         break;
      }
      case XCB_INPUT_RAW_MOTION: {
         xcb_input_raw_motion_event_t *rawEvent = reinterpret_cast<xcb_input_raw_motion_event_t *>(event);
         if (m_currentInhabitedWindow != 0) {
            PackageDeltaEvent(rawEvent);
         }
         break;
      }
   }
}

void MasterPointerX11::BindToWindow(const Window *const window) {
   m_boundWindow = static_cast<const WindowX11 *const>(window)->GetX11WindowID();
   if (!AttemptCursorGrab(m_boundWindow)) {
      m_attemptGrabNextPoll = true;
   }
}

void MasterPointerX11::UnbindFromWindow() {
   m_boundWindow = 0;
   m_attemptGrabNextPoll = false;
   auto cookie = xcb_ungrab_pointer_checked(m_connection, XCB_CURRENT_TIME);
   // TODO: This doesn't work unless I do a request check. What??
   auto err = xcb_request_check(m_connection, cookie);
}

MasterPointerX11::MasterPointerX11() : PointerDeviceX11(GetMasterPointerDeviceID()) {
   if (m_instantiated) {
      throw MultipleMasterPointerError();
   }
   m_corePointerID = GetMasterPointerDeviceID();
   if (m_corePointerID == 0) {
      throw InputDeviceFailure();
   }
}

xcb_input_device_id_t MasterPointerX11::GetMasterPointerDeviceID() {
   XConnection::CreateConnection();
   xcb_connection_t *connection = XConnection::GetConnection();
   xcb_input_xi_query_device_cookie_t queryCookie =
      xcb_input_xi_query_device(connection, XCB_INPUT_DEVICE_ALL_MASTER);
   xcb_input_xi_query_device_reply_t *reply =
      xcb_input_xi_query_device_reply(connection, queryCookie, nullptr);

   xcb_input_xi_device_info_iterator_t iter = xcb_input_xi_query_device_infos_iterator(reply);
   while (iter.rem > 0) {
      auto element = iter.data;
      if (element->type == XCB_INPUT_DEVICE_TYPE_MASTER_POINTER) {
         free(reply);
         return element->deviceid;
      }
      xcb_input_xi_device_info_next(&iter);
   }
   return 0;
}