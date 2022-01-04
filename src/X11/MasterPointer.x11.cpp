#include "MasterPointer.x11.hpp"

#include <cstring>

#include "EventDispatcher.x11.hpp"
#include "InputDevice.x11.hpp"
#include "InputMapper.x11.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "Window.x11.hpp"
#include "X11/extensions/XInput2.h"
#include "XConnection.h"

using namespace NLSWIN;

bool MasterPointerX11::m_instantiated {false};

MasterPointer &MasterPointer::GetMasterPointer() {
   static std::shared_ptr<MasterPointerX11> instance = nullptr;
   if (!instance) {
      instance = std::make_shared<MasterPointerX11>();
      EventDispatcherX11::RegisterListener(instance);
   }
   return *instance;
}

void MasterPointerX11::SetCursorInvisible() {
   // Only hide the cursor if the pointer is currently within bounds of a window.
   // Don't send another request if the cursor is already hidden - the requests appear to stack.
   if (m_clientRequestedHiddenCursor && m_currentInhabitedWindow && !m_cursorHidden) {
      xcb_xfixes_hide_cursor(m_connection, m_currentInhabitedWindow);
      xcb_flush(m_connection);
      m_cursorHidden = true;
   }
}

void MasterPointerX11::ShowCursor() noexcept {
   m_clientRequestedHiddenCursor = false;
   SetCursorVisible();
}
void MasterPointerX11::HideCursor() noexcept {
   m_clientRequestedHiddenCursor = true;
   // If the cursor is not currently within the bounds of a window, this call will do nothing.
   // The cursor will not be set to hidden until it next enters the bounds of a window (EnterEvent).
   SetCursorInvisible();
}

void MasterPointerX11::SetCursorVisible() {
   if (m_cursorHidden) {
      for (auto window: m_subscribedWindows) { xcb_xfixes_show_cursor_checked(m_connection, window.first); }
      xcb_flush(m_connection);
      m_cursorHidden = false;
   }
}

void MasterPointerX11::ProcessXInputEvent(xcb_ge_generic_event_t *event) {
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
            SetCursorInvisible();
         }
         break;
      }
      case XCB_INPUT_LEAVE: {
         xcb_input_leave_event_t *leaveEvent = reinterpret_cast<xcb_input_leave_event_t *>(event);
         if (!m_boundWindow) {
            PackageLeaveEvent(leaveEvent);
            SetCursorVisible();
         }
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
         // Hacky workaround to ignore duplicate motion events due to master/slave pointer issues.
         static xcb_timestamp_t lastTimeStamp = 0;
         if (rawEvent->time == lastTimeStamp) {
            break;
         }
         lastTimeStamp = rawEvent->time;
         if (m_currentInhabitedWindow != 0) {
            PackageDeltaEvent(rawEvent);
         }
         break;
      }
   }
}

void MasterPointerX11::BindToWindow(const Window *const window) noexcept {
   m_boundWindow = static_cast<const WindowX11 *const>(window)->GetX11WindowID();
   AttemptCursorGrab(m_boundWindow);
   xcb_set_input_focus(m_connection, 0, m_boundWindow, XCB_CURRENT_TIME);
   xcb_flush(m_connection);
}

void MasterPointerX11::UnbindFromWindow() noexcept {
   if (m_boundWindow = 0) {
      return;
   }
   m_boundWindow = 0;
   auto cookie = xcb_ungrab_pointer_checked(m_connection, XCB_CURRENT_TIME);
   auto err = xcb_request_check(m_connection, cookie);
}

void MasterPointerX11::UnsubscribeFromWindow(xcb_window_t window) {
   if (m_subscribedWindows.count(window)) {
      m_subscribedWindows.erase(window);
   }
}

MasterPointerX11::MasterPointerX11() : PointerDeviceX11(GetMasterPointerDeviceID()) {
   if (m_instantiated) {
      throw MultipleMasterPointerError();
   }
   m_instantiated = true;
   m_corePointerID = GetMasterPointerDeviceID();
   if (m_corePointerID == 0) {
      throw InputDeviceFailure();
   }
}

xcb_input_device_id_t MasterPointerX11::GetMasterPointerDeviceID() {
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