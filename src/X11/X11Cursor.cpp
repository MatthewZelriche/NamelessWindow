#include "X11Cursor.hpp"

#include <X11/X.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "NamelessWindow/Exceptions.hpp"
#include "X11EventBus.hpp"
#include "X11Window.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::shared_ptr<NLSWIN::Cursor> NLSWIN::Cursor::Create() {
   std::shared_ptr<X11Cursor> impl = std::make_shared<X11Cursor>();
   X11EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

X11Cursor::X11Cursor() {
   static bool m_isInstantiated = false;
   if (m_isInstantiated) {
      throw MultipleCursorException();
   }
   m_deviceID = GetMasterPointerDeviceID();
   xcb_pixmap_t pixmap = xcb_generate_id(XConnection::GetConnection());
   xcb_create_cursor(XConnection::GetConnection(), m_cursor, pixmap, pixmap, 0, 0, 0, 0, 0, 0, 0, 0);
   SubscribeToRawRootEvents(m_rawInputEventMask);
}

void X11Cursor::Show() noexcept {
   m_requestedHidden = false;
   AttemptSetVisible();
}

void X11Cursor::Hide() noexcept {
   m_requestedHidden = true;
   if (X11Window::IsUserWindow(m_inhabitedWindow)) {
      AttemptSetHidden();
   }
}

void X11Cursor::AttemptSetVisible() {
   if (m_isHidden) {
      auto cookie = xcb_xfixes_show_cursor_checked(XConnection::GetConnection(), m_inhabitedWindow);
      auto err = xcb_request_check(XConnection::GetConnection(), cookie);
      if (err) {
         // Probably tried to pass a window id that just became invalid (ex the window closed while an
         // invisible cursor was inside it), should hopefully be fixed almost immediately on entering the next
         // valid window.
         return;
      }
      m_isHidden = false;
      xcb_flush(XConnection::GetConnection());
   }
}

void X11Cursor::AttemptSetHidden() {
   if (!m_isHidden) {
      auto cookie = xcb_xfixes_hide_cursor_checked(XConnection::GetConnection(), m_inhabitedWindow);
      m_isHidden = true;
      xcb_flush(XConnection::GetConnection());
   }
}

void X11Cursor::ProcessGenericEvent(xcb_generic_event_t *event) {
   switch (event->response_type & ~0x80) {
      case XCB_BUTTON_PRESS: {
         xcb_button_press_event_t *buttonPressEvent = reinterpret_cast<xcb_button_press_event_t *>(event);
         PushEvent(PackageNewButtonPressEvent(buttonPressEvent, buttonPressEvent->event));
         break;
      }
      case XCB_BUTTON_RELEASE: {
         xcb_button_release_event_t *buttonReleaseEvent =
            reinterpret_cast<xcb_button_release_event_t *>(event);
         PushEvent(PackageNewButtonReleaseEvent(buttonReleaseEvent, buttonReleaseEvent->event));
         break;
      }
      case XCB_ENTER_NOTIFY: {
         xcb_enter_notify_event_t *enterEvent = reinterpret_cast<xcb_enter_notify_event_t *>(event);
         m_inhabitedWindow = enterEvent->event;
         if (X11Window::IsUserWindow(m_inhabitedWindow)) {
            if (m_requestedHidden && !m_isHidden) {
               AttemptSetHidden();
            }
         }
         break;
      }
      case XCB_LEAVE_NOTIFY: {
         xcb_leave_notify_event_t *leaveEvent = reinterpret_cast<xcb_leave_notify_event_t *>(event);
         if (m_boundWindow) {
            if (leaveEvent->mode == XCB_INPUT_NOTIFY_MODE_UNGRAB) {
               AttemptSetVisible();
            }
         } else {
            AttemptSetVisible();
            m_inhabitedWindow = 0;
         }
         break;
      }
      case XCB_FOCUS_OUT: {
         xcb_focus_out_event_t *focusOutEvent = reinterpret_cast<xcb_focus_out_event_t *>(event);
         if (focusOutEvent->event == m_boundWindow) {
            xcb_ungrab_pointer(XConnection::GetConnection(), XCB_CURRENT_TIME);
            xcb_flush(XConnection::GetConnection());
            m_isTempUnbound = true;
         }
         break;
      }
      case XCB_FOCUS_IN: {
         xcb_focus_in_event_t *focusInEvent = reinterpret_cast<xcb_focus_in_event_t *>(event);
         if (focusInEvent->event == m_boundWindow && m_isTempUnbound) {
            auto cookie = xcb_grab_pointer(XConnection::GetConnection(), false, m_boundWindow, m_xcbEventMask,
                                           XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, m_boundWindow, m_cursor,
                                           XCB_CURRENT_TIME);
            xcb_flush(XConnection::GetConnection());
            m_isTempUnbound = false;
         }
         break;
      }
      case XCB_MOTION_NOTIFY: {
         xcb_motion_notify_event_t *motionEvent = reinterpret_cast<xcb_motion_notify_event_t *>(event);
         PushEvent(PackageNewMoveEvent(motionEvent, motionEvent->event));
         break;
      }
      case XCB_DESTROY_NOTIFY: {
         if (m_boundWindow) {
            xcb_destroy_notify_event_t *destroyEvent = reinterpret_cast<xcb_destroy_notify_event_t *>(event);
            if (m_boundWindow == destroyEvent->event) {
               Free();
            }
         }
         break;
      }
      case XCB_GE_GENERIC: {
         xcb_ge_generic_event_t *genericEvent = reinterpret_cast<xcb_ge_generic_event_t *>(event);
         switch (genericEvent->event_type) {
            case XCB_INPUT_RAW_MOTION: {
               xcb_input_raw_motion_event_t *rawEvent =
                  reinterpret_cast<xcb_input_raw_motion_event_t *>(event);
               // Hacky workaround to ignore duplicate motion events due to master/slave pointer issues.
               static xcb_timestamp_t lastTimeStamp = 0;
               if (rawEvent->time == lastTimeStamp) {
                  break;
               }
               lastTimeStamp = rawEvent->time;
               PushEvent(PackageNewDeltaEvents(rawEvent));
               break;
            }
         }
      }
   }
}

void X11Cursor::Free() noexcept {
   xcb_ungrab_pointer(XConnection::GetConnection(), XCB_CURRENT_TIME);
   m_boundWindow = 0;
   m_isTempUnbound = false;
   xcb_flush(XConnection::GetConnection());
}

void X11Cursor::Confine(Window *window) noexcept {
   const X11Window *const x11Window = reinterpret_cast<const X11Window *const>(window);
   // Always unbind the cursor first
   Free();
   auto cookie = xcb_grab_pointer(XConnection::GetConnection(), false, x11Window->GetX11ID(), m_xcbEventMask,
                                  XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, x11Window->GetX11ID(), m_cursor,
                                  XCB_CURRENT_TIME);

   m_boundWindow = x11Window->GetX11ID();
   xcb_flush(XConnection::GetConnection());
}

Event X11Cursor::PackageNewButtonPressEvent(xcb_button_press_event_t *event, xcb_window_t sourceWindow) {
   if (event->detail >= 4 && event->detail <= 7) {
      MouseScrollEvent scrollEvent;
      scrollEvent.scrollType = (ScrollType)(event->detail - 4);
      scrollEvent.sourceWindow = X11Window::IDFromHWND(sourceWindow);
      return scrollEvent;
   }
   MouseButtonEvent mouseButtonEvent;
   mouseButtonEvent.button = TranslateButton(event->detail);
   mouseButtonEvent.type = ButtonPressType::PRESSED;
   mouseButtonEvent.xPos = event->event_x;
   mouseButtonEvent.yPos = event->event_y;
   mouseButtonEvent.sourceWindow = X11Window::IDFromHWND(sourceWindow);
   return mouseButtonEvent;
}

Event X11Cursor::PackageNewButtonReleaseEvent(xcb_button_press_event_t *event, xcb_window_t sourceWindow) {
   MouseButtonEvent mouseButtonEvent;
   // Only process scroll events on a button press - processing both on press and release gives
   // erroneous events.
   if (event->detail >= 4 && event->detail <= 7) {
      return std::monostate();
   }
   mouseButtonEvent.button = TranslateButton(event->detail);
   mouseButtonEvent.type = ButtonPressType::RELEASED;
   mouseButtonEvent.xPos = event->event_x;
   mouseButtonEvent.yPos = event->event_y;
   mouseButtonEvent.sourceWindow = X11Window::IDFromHWND(sourceWindow);
   return mouseButtonEvent;
}

Event X11Cursor::PackageNewMoveEvent(xcb_motion_notify_event_t *event, xcb_window_t sourceWindow) {
   float newX = event->event_x;
   float newY = event->event_y;
   // Don't send an event if we've somehow recieved a motion event yet we havent moved.
   // (for example, when using the scroll wheel ??)
   if (newX == lastX && newY == lastY) {
      return std::monostate();
   }
   MouseMovementEvent moveEvent;
   moveEvent.newXPos = newX;
   moveEvent.newYPos = newY;
   moveEvent.sourceWindow = X11Window::IDFromHWND(sourceWindow);
   lastX = newX;
   lastY = newY;
   return moveEvent;
}

xcb_input_device_id_t X11Cursor::GetMasterPointerDeviceID() noexcept {
   xcb_input_xi_query_device_cookie_t queryCookie =
      xcb_input_xi_query_device(XConnection::GetConnection(), XCB_INPUT_DEVICE_ALL_MASTER);
   xcb_input_xi_query_device_reply_t *reply =
      xcb_input_xi_query_device_reply(XConnection::GetConnection(), queryCookie, nullptr);

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