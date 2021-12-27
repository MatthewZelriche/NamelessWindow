#include "Pointer.x11.hpp"

#include <cstring>

#include "EventQueue.x11.hpp"
#include "InputDevice.x11.hpp"
#include "InputMapper.x11.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "Window.x11.hpp"
#include "X11/extensions/XInput2.h"
#include "XConnection.h"

using namespace NLSWIN;

#include <iostream>
void PointerX11::ProcessXInputEvent(xcb_ge_generic_event_t *event, xcb_window_t x11WindowID,
                                    EventListenerX11 *listener) {
   switch (event->event_type) {
      case XCB_INPUT_BUTTON_PRESS: {
         xcb_input_button_press_event_t *buttonPressEvent =
            reinterpret_cast<xcb_input_button_press_event_t *>(event);
         if (buttonPressEvent->event == x11WindowID) {
            // Only process scroll events on a button press - processing both on press and release gives
            // duplicate events.
            // TODO: Look into continuous scroll values through valuators.
            if (buttonPressEvent->detail >= 4 && buttonPressEvent->detail <= 7) {
               MouseScrollEvent scrollEvent;
               scrollEvent.scrollType = (ScrollType)(buttonPressEvent->detail - 4);
               scrollEvent.xPos = TranslateXCBFloat(buttonPressEvent->event_x);
               scrollEvent.yPos = TranslateXCBFloat(buttonPressEvent->event_y);
               listener->PushEvent(scrollEvent);
               break;
            }
            MouseButtonEvent mouseButtonEvent;
            mouseButtonEvent.button = X11InputMapper::TranslateButton(buttonPressEvent->detail);
            mouseButtonEvent.type = ButtonPressType::PRESSED;
            mouseButtonEvent.xPos = TranslateXCBFloat(buttonPressEvent->event_x);
            mouseButtonEvent.yPos = TranslateXCBFloat(buttonPressEvent->event_y);
            listener->PushEvent(mouseButtonEvent);
         }
         break;
      }
      case XCB_INPUT_BUTTON_RELEASE: {
         xcb_input_button_release_event_t *buttonReleaseEvent =
            reinterpret_cast<xcb_input_button_release_event_t *>(event);
         if (buttonReleaseEvent->event == x11WindowID) {
            MouseButtonEvent mouseButtonEvent;
            mouseButtonEvent.button = X11InputMapper::TranslateButton(buttonReleaseEvent->detail);
            mouseButtonEvent.type = ButtonPressType::RELEASED;
            mouseButtonEvent.xPos = TranslateXCBFloat(buttonReleaseEvent->event_x);
            mouseButtonEvent.yPos = TranslateXCBFloat(buttonReleaseEvent->event_y);
            listener->PushEvent(mouseButtonEvent);
         }
         break;
      }
      case XCB_INPUT_ENTER: {
         xcb_input_enter_event_t *enterEvent = reinterpret_cast<xcb_input_enter_event_t *>(event);
         if (enterEvent->event == x11WindowID) {
            MouseEnterEvent mouseEnterEvent;
            mouseEnterEvent.xPos = TranslateXCBFloat(enterEvent->event_x);
            mouseEnterEvent.yPos = TranslateXCBFloat(enterEvent->event_y);
            m_currentInhabitedWindow = enterEvent->event;
            listener->PushEvent(mouseEnterEvent);
            lastX = mouseEnterEvent.xPos;
            lastY = mouseEnterEvent.yPos;
         }
         break;
      }
      case XCB_INPUT_LEAVE: {
         xcb_input_leave_event_t *leaveEvent = reinterpret_cast<xcb_input_leave_event_t *>(event);
         if (leaveEvent->event == x11WindowID) {
            m_currentInhabitedWindow = 0;
         }
         break;
      }
      case XCB_INPUT_MOTION: {
         xcb_input_motion_event_t *motionEvent = reinterpret_cast<xcb_input_motion_event_t *>(event);
         if (motionEvent->event == x11WindowID) {
            float newX = TranslateXCBFloat(motionEvent->event_x);
            float newY = TranslateXCBFloat(motionEvent->event_y);
            // Don't send an event if we've somehow recieved a motion event yet we havent moved.
            // (for example, when using the scroll wheel ??)
            if (newX == lastX && newY == lastY) {
               return;
            }
            MouseMovementEvent moveEvent;
            moveEvent.newXPos = newX;
            moveEvent.newYPos = newY;
            lastX = newX;
            lastY = newY;
            listener->PushEvent(moveEvent);
         }
         break;
      }
      case XCB_INPUT_RAW_MOTION: {
         // Why are motion events seemingly considered button press events when accessing evaluators?
         if (m_currentInhabitedWindow != 0) {
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
            listener->PushEvent(rawDeltaMoveEvent);
            //  Non-raw axisvalues gives us the accelerated delta.
            auto axisValues =
               xcb_input_raw_button_press_axisvalues((xcb_input_raw_button_press_event_t *)rawMotion);
            MouseDeltaMovementEvent deltaMoveEvent;
            deltaMoveEvent.deltaX = TranslateXCBFloat(axisValues[0]);
            deltaMoveEvent.deltaY = TranslateXCBFloat(axisValues[1]);
            std::cout << rawAxisValues[0].frac << std::endl;
            listener->PushEvent(deltaMoveEvent);
         }
         break;
      }
   }
}

PointerX11::PointerX11() {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();
   m_corePointerID = GetMasterPointerDeviceID(m_connection);
   if (m_corePointerID == 0) {
      throw InputDeviceFailure();
   }
}

xcb_input_device_id_t PointerX11::GetMasterPointerDeviceID(xcb_connection_t *connection) {
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