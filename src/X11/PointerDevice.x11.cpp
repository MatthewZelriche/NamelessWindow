#include "PointerDevice.x11.hpp"

#include <xcb/xfixes.h>

#include "InputMapper.x11.hpp"

using namespace NLSWIN;

PointerDeviceX11::PointerDeviceX11(xcb_input_device_id_t deviceID) {
   m_deviceID = deviceID;

   xcb_cursor_t cursor = xcb_generate_id(m_connection);
   xcb_pixmap_t pixmap = xcb_generate_id(m_connection);
   xcb_create_cursor(m_connection, m_cursor, pixmap, pixmap, 0, 0, 0, 0, 0, 0, 0, 0);
   SubscribeToRawRootEvents(XCB_INPUT_XI_EVENT_MASK_RAW_MOTION);
}

void PointerDeviceX11::PackageButtonPressEvent(xcb_input_button_press_event_t *event) {
   if (m_subscribedWindows.count(event->event)) {
      // Only process scroll events on a button press - processing both on press and release gives
      // duplicate events.
      if (event->detail >= 4 && event->detail <= 7) {
         MouseScrollEvent scrollEvent;
         scrollEvent.scrollType = (ScrollType)(event->detail - 4);
         scrollEvent.xPos = TranslateXCBFloat(event->event_x);
         scrollEvent.yPos = TranslateXCBFloat(event->event_y);
         scrollEvent.sourceWindow = m_subscribedWindows[event->event];
         PushEvent(scrollEvent);
         return;
      }
      MouseButtonEvent mouseButtonEvent;
      mouseButtonEvent.button = X11InputMapper::TranslateButton(event->detail);
      mouseButtonEvent.type = ButtonPressType::PRESSED;
      mouseButtonEvent.xPos = TranslateXCBFloat(event->event_x);
      mouseButtonEvent.yPos = TranslateXCBFloat(event->event_y);
      mouseButtonEvent.sourceWindow = m_subscribedWindows[event->event];
      PushEvent(mouseButtonEvent);
   }
}
void PointerDeviceX11::PackageButtonReleaseEvent(xcb_input_button_release_event_t *event) {
   if (m_subscribedWindows.count(event->event)) {
      MouseButtonEvent mouseButtonEvent;
      mouseButtonEvent.button = X11InputMapper::TranslateButton(event->detail);
      mouseButtonEvent.type = ButtonPressType::RELEASED;
      mouseButtonEvent.xPos = TranslateXCBFloat(event->event_x);
      mouseButtonEvent.yPos = TranslateXCBFloat(event->event_y);
      mouseButtonEvent.sourceWindow = m_subscribedWindows[event->event];
      PushEvent(mouseButtonEvent);
   }
}

void PointerDeviceX11::PackageEnterEvent(xcb_input_enter_event_t *event) {
   if (m_subscribedWindows.count(event->event)) {
      MouseEnterEvent mouseEnterEvent;
      mouseEnterEvent.xPos = TranslateXCBFloat(event->event_x);
      mouseEnterEvent.yPos = TranslateXCBFloat(event->event_y);
      m_currentInhabitedWindow = event->event;
      mouseEnterEvent.sourceWindow = m_subscribedWindows[event->event];
      PushEvent(mouseEnterEvent);
      lastX = mouseEnterEvent.xPos;
      lastY = mouseEnterEvent.yPos;
   }
}

void PointerDeviceX11::PackageLeaveEvent(xcb_input_leave_event_t *event) {
   if (m_subscribedWindows.count(event->event)) {
      m_currentInhabitedWindow = 0;
      MouseLeaveEvent mouseLeaveEvent;
      mouseLeaveEvent.xPos = TranslateXCBFloat(event->event_x);
      mouseLeaveEvent.yPos = TranslateXCBFloat(event->event_y);
      mouseLeaveEvent.sourceWindow = m_subscribedWindows[event->event];
      lastX = mouseLeaveEvent.xPos;
      lastY = mouseLeaveEvent.yPos;
      PushEvent(mouseLeaveEvent);
   }
}

void PointerDeviceX11::PackageMotionEvent(xcb_input_motion_event_t *event) {
   if (m_subscribedWindows.count(event->event)) {
      float newX = TranslateXCBFloat(event->event_x);
      float newY = TranslateXCBFloat(event->event_y);
      // Don't send an event if we've somehow recieved a motion event yet we havent moved.
      // (for example, when using the scroll wheel ??)
      if (newX == lastX && newY == lastY) {
         return;
      }
      MouseMovementEvent moveEvent;
      moveEvent.newXPos = newX;
      moveEvent.newYPos = newY;
      moveEvent.sourceWindow = m_subscribedWindows[event->event];
      lastX = newX;
      lastY = newY;
      PushEvent(moveEvent);
   }
}

void PointerDeviceX11::AttemptCursorGrab(xcb_window_t window) {
   auto cookie = xcb_grab_pointer(m_connection, 1, m_boundWindow, 0, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
                                  m_boundWindow, m_cursor, XCB_CURRENT_TIME);
   auto reply = xcb_grab_pointer_reply(m_connection, cookie, nullptr);
   auto status = reply->status;
   free(reply);
   xcb_flush(m_connection);
   // If we fail to grab because the window isn't visible, this will notify the bound window to bind as soon
   // as it becomes visible
   m_grabOnNextView = status == XCB_GRAB_STATUS_NOT_VIEWABLE;
}

void PointerDeviceX11::PackageDeltaEvent(xcb_input_raw_motion_event_t *event) {
   xcb_input_raw_motion_event_t *rawMotion = reinterpret_cast<xcb_input_raw_motion_event_t *>(event);
   xcb_input_raw_button_press_event_t *rawEvent =
      reinterpret_cast<xcb_input_raw_button_press_event_t *>(rawMotion);

   // Ignore other raw motions, based on the valuator indices they contain. This is the best way
   // I can think of for disregarding scroll raw motion events. Indices 0 and 1 appear to be
   // real mouse motion events, while 2 and 3 appear to be horz/vertical valuators for scroll.
   auto mask = xcb_input_raw_button_press_valuator_mask(rawEvent);
   if ((mask[0] & (1 << 2)) || (mask[0] & (1 << 3))) {
      return;
   }
   auto rawAxisValues =
      xcb_input_raw_button_press_axisvalues_raw((xcb_input_raw_button_press_event_t *)rawMotion);
   MouseRawDeltaMovementEvent rawDeltaMoveEvent;
   rawDeltaMoveEvent.deltaX = TranslateXCBFloat(rawAxisValues[0]);
   rawDeltaMoveEvent.deltaY = TranslateXCBFloat(rawAxisValues[1]);
   PushEvent(rawDeltaMoveEvent);
   //  Non-raw axisvalues gives us the accelerated delta.
   auto axisValues = xcb_input_raw_button_press_axisvalues((xcb_input_raw_button_press_event_t *)rawMotion);
   MouseDeltaMovementEvent deltaMoveEvent;
   deltaMoveEvent.deltaX = TranslateXCBFloat(axisValues[0]);
   deltaMoveEvent.deltaY = TranslateXCBFloat(axisValues[1]);

   PushEvent(deltaMoveEvent);
}