#include "X11Cursor.hpp"

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

void X11Cursor::ShowCursor() noexcept {
   m_requestedHidden = false;
   AttemptSetVisible();
}

void X11Cursor::HideCursor() noexcept {
   m_requestedHidden = true;
   if (WithinSubscribedWindow()) {
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
         if (!m_boundWindow) {
            PushEvent(PackageNewButtonPressEvent(buttonPressEvent, buttonPressEvent->event));
         } else {
            if (GetSubscribedWindows().count(buttonPressEvent->event)) {
               PushEvent(PackageNewButtonPressEvent(buttonPressEvent, m_boundWindow));
            }
         }
         break;
      }
      case XCB_BUTTON_RELEASE: {
         xcb_button_release_event_t *buttonReleaseEvent =
            reinterpret_cast<xcb_button_release_event_t *>(event);
         if (!m_boundWindow) {
            PushEvent(PackageNewButtonReleaseEvent(buttonReleaseEvent, buttonReleaseEvent->event));
         } else {
            if (GetSubscribedWindows().count(buttonReleaseEvent->event)) {
               PushEvent(PackageNewButtonReleaseEvent(buttonReleaseEvent, m_boundWindow));
            }
         }
         break;
      }
      case XCB_ENTER_NOTIFY: {
         xcb_enter_notify_event_t *enterEvent = reinterpret_cast<xcb_enter_notify_event_t *>(event);
         m_inhabitedWindow = enterEvent->event;
         if (GetSubscribedWindows().count(m_inhabitedWindow)) {
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
               m_boundWindow = 0;
               AttemptSetVisible();
            }
         } else {
            AttemptSetVisible();
            m_inhabitedWindow = 0;
         }
         break;
      }
      case XCB_MOTION_NOTIFY: {
         xcb_motion_notify_event_t *motionEvent = reinterpret_cast<xcb_motion_notify_event_t *>(event);
         if (!m_boundWindow) {
            PushEvent(PackageNewMoveEvent(motionEvent, motionEvent->event));
         } else {
            if (GetSubscribedWindows().count(motionEvent->event)) {
               PushEvent(PackageNewMoveEvent(motionEvent, m_boundWindow));
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
               if (GetSubscribedWindows().count(m_inhabitedWindow)) {
                  auto deltaEvents = PackageNewDeltaEvents(rawEvent);
                  PushEvent(deltaEvents.first);
                  PushEvent(deltaEvents.second);
               }
               break;
            }
         }
      }
   }
}

bool X11Cursor::WithinSubscribedWindow() const {
   return GetSubscribedWindows().count(m_inhabitedWindow);
}

void X11Cursor::BindToWindow(const Window *const window) noexcept {
   const X11Window *const x11Window = reinterpret_cast<const X11Window *const>(window);
   // Always unbind the cursor first
   xcb_ungrab_pointer(XConnection::GetConnection(), XCB_CURRENT_TIME);
   auto cookie = xcb_grab_pointer(XConnection::GetConnection(), false, x11Window->GetX11ID(), m_xcbEventMask,
                                  XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, x11Window->GetX11ID(), m_cursor,
                                  XCB_CURRENT_TIME);

   m_boundWindow = x11Window->GetX11ID();
   xcb_set_input_focus(XConnection::GetConnection(), 0, m_boundWindow, XCB_CURRENT_TIME);
   xcb_flush(XConnection::GetConnection());
}

Event X11Cursor::PackageNewButtonPressEvent(xcb_button_press_event_t *event, xcb_window_t sourceWindow) {
   if (event->detail >= 4 && event->detail <= 7) {
      MouseScrollEvent scrollEvent;
      scrollEvent.scrollType = (ScrollType)(event->detail - 4);
      scrollEvent.sourceWindow = GetSubscribedWindows().at(sourceWindow).lock()->GetGenericID();
      return scrollEvent;
   }
   MouseButtonEvent mouseButtonEvent;
   mouseButtonEvent.button = TranslateButton(event->detail);
   mouseButtonEvent.type = ButtonPressType::PRESSED;
   mouseButtonEvent.sourceWindow = GetSubscribedWindows().at(sourceWindow).lock()->GetGenericID();
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
   mouseButtonEvent.sourceWindow = GetSubscribedWindows().at(sourceWindow).lock()->GetGenericID();
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
   moveEvent.sourceWindow = GetSubscribedWindows().at(event->event).lock()->GetGenericID();
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