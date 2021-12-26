#include "Pointer.x11.hpp"

#include <cstring>

#include "EventQueue.x11.hpp"
#include "InputDevice.x11.hpp"
#include "InputMapper.x11.hpp"
#include "Window.x11.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::vector<PointerDeviceInfo> Pointer::EnumeratePointers() noexcept {
   std::vector<PointerDeviceInfo> pointers;
   // Open a brief temporary connection to get the screens
   xcb_connection_t *connection = xcb_connect(nullptr, nullptr);
   int result = xcb_connection_has_error(connection);
   if (result != 0) {
      xcb_disconnect(connection);
      return {};
   }
   xcb_input_xi_query_device_cookie_t queryCookie =
      xcb_input_xi_query_device(connection, XCB_INPUT_DEVICE_ALL);
   xcb_input_xi_query_device_reply_t *reply =
      xcb_input_xi_query_device_reply(connection, queryCookie, nullptr);

   xcb_input_xi_device_info_iterator_t iter = xcb_input_xi_query_device_infos_iterator(reply);
   while (iter.rem > 0) {
      auto element = iter.data;
      if (element->enabled) {
         if (element->type == XCB_INPUT_DEVICE_TYPE_SLAVE_POINTER) {
            // Ignore xtest devices
            const char *name = xcb_input_xi_device_info_name(element);
            if (!std::strstr(name, "XTEST")) {
               PointerDeviceInfo dev {name, element->deviceid};
               pointers.push_back(dev);
            }
         }
      }
      xcb_input_xi_device_info_next(&iter);
   }
   free(reply);
   return pointers;
}

void Pointer::Impl::ProcessXInputEvent(xcb_ge_generic_event_t *event) {
   switch (event->event_type) {
      case XCB_INPUT_BUTTON_PRESS: {
         xcb_input_button_press_event_t *buttonPressEvent =
            reinterpret_cast<xcb_input_button_press_event_t *>(event);
         if (m_SubscribedWindows.count(buttonPressEvent->event)) {
            if (GetDeviceID() == buttonPressEvent->deviceid || GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
               // Only process scroll events on a button press - processing both on press and release gives
               // duplicate events.
               // TODO: Look into continuous scroll values through valuators.
               if (buttonPressEvent->detail >= 4 && buttonPressEvent->detail <= 7) {
                  MouseScrollEvent scrollEvent;
                  scrollEvent.scrollType = (ScrollType)(buttonPressEvent->detail - 4);
                  scrollEvent.xPos = TranslateXCBFloat(buttonPressEvent->event_x);
                  scrollEvent.yPos = TranslateXCBFloat(buttonPressEvent->event_y);
                  PushEvent(scrollEvent);
                  return;
               }
               MouseButtonEvent mouseButtonEvent;
               mouseButtonEvent.button = X11InputMapper::TranslateButton(buttonPressEvent->detail);
               mouseButtonEvent.type = ButtonPressType::PRESSED;
               mouseButtonEvent.xPos = TranslateXCBFloat(buttonPressEvent->event_x);
               mouseButtonEvent.yPos = TranslateXCBFloat(buttonPressEvent->event_y);
               PushEvent(mouseButtonEvent);
            }
         }
         break;
      }
      case XCB_INPUT_BUTTON_RELEASE: {
         xcb_input_button_release_event_t *buttonReleaseEvent =
            reinterpret_cast<xcb_input_button_release_event_t *>(event);
         if (m_SubscribedWindows.count(buttonReleaseEvent->event)) {
            if (GetDeviceID() == buttonReleaseEvent->deviceid ||
                GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
               MouseButtonEvent mouseButtonEvent;
               mouseButtonEvent.button = X11InputMapper::TranslateButton(buttonReleaseEvent->detail);
               mouseButtonEvent.type = ButtonPressType::RELEASED;
               mouseButtonEvent.xPos = TranslateXCBFloat(buttonReleaseEvent->event_x);
               mouseButtonEvent.yPos = TranslateXCBFloat(buttonReleaseEvent->event_y);
               PushEvent(mouseButtonEvent);
            }
         }
         break;
      }
      case XCB_INPUT_ENTER: {
         xcb_input_enter_event_t *enterEvent = reinterpret_cast<xcb_input_enter_event_t *>(event);
         if (m_SubscribedWindows.count(enterEvent->event)) {
            if (GetDeviceID() == enterEvent->deviceid || GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
               if (enterEvent->mode == XCB_NOTIFY_MODE_NORMAL) {
                  MouseEnterEvent mouseEnterEvent;
                  mouseEnterEvent.xPos = TranslateXCBFloat(enterEvent->event_x);
                  mouseEnterEvent.yPos = TranslateXCBFloat(enterEvent->event_y);
                  m_currentInhabitedWindow = enterEvent->event;
                  PushEvent(mouseEnterEvent);
               }
            }
         }
         break;
      }
      case XCB_INPUT_LEAVE: {
         xcb_input_leave_event_t *leaveEvent = reinterpret_cast<xcb_input_leave_event_t *>(event);
         if (m_SubscribedWindows.count(leaveEvent->event)) {
            if (GetDeviceID() == leaveEvent->deviceid || GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
               m_currentInhabitedWindow = 0;
            }
         }
      }
      case XCB_INPUT_MOTION: {
         xcb_input_motion_event_t *motionEvent = reinterpret_cast<xcb_input_motion_event_t *>(event);
         if (m_SubscribedWindows.count(motionEvent->event)) {
            if (GetDeviceID() == motionEvent->deviceid || GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
               float newX = TranslateXCBFloat(motionEvent->event_x);
               float newY = TranslateXCBFloat(motionEvent->event_y);
               MouseMovementEvent moveEvent;
               moveEvent.newXPos = newX;
               moveEvent.newYPos = newY;
               PushEvent(moveEvent);
            }
         }
         break;
      }
      case XCB_INPUT_RAW_MOTION: {
         // Why are motion events seemingly considered button press events when accessing evaluators?
         if (m_SubscribedWindows.count(m_currentInhabitedWindow)) {
            xcb_input_raw_motion_event_t *rawMotion = reinterpret_cast<xcb_input_raw_motion_event_t *>(event);
            if (GetDeviceID() == rawMotion->deviceid || GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
               auto rawAxisValues =
                  xcb_input_raw_button_press_axisvalues_raw((xcb_input_raw_button_press_event_t *)rawMotion);
               MouseRawDeltaMovementEvent rawDeltaMoveEvent;
               rawDeltaMoveEvent.deltaX = TranslateXCBFloat(rawAxisValues[0]);
               rawDeltaMoveEvent.deltaY = TranslateXCBFloat(rawAxisValues[1]);
               PushEvent(rawDeltaMoveEvent);
               // Non-raw axisvalues gives us the accelerated delta.
               auto axisValues =
                  xcb_input_raw_button_press_axisvalues((xcb_input_raw_button_press_event_t *)rawMotion);
               MouseDeltaMovementEvent deltaMoveEvent;
               deltaMoveEvent.deltaX = TranslateXCBFloat(axisValues[0]);
               deltaMoveEvent.deltaY = TranslateXCBFloat(axisValues[1]);
               PushEvent(deltaMoveEvent);
            }
         }
         break;
      }
   }
}

Pointer::Pointer() : m_pImpl(std::make_shared<Pointer::Impl>()) {
   EventQueueX11::RegisterListener(m_pImpl);
}

Pointer::Pointer(PointerDeviceInfo device) : m_pImpl(std::make_shared<Pointer::Impl>(device)) {
   EventQueueX11::RegisterListener(m_pImpl);
}

Pointer::~Pointer() {
}

Pointer::Impl::Impl() : Impl(2) {
}

Pointer::Impl::Impl(PointerDeviceInfo device) : Impl(device.platformSpecificIdentifier) {
}

Pointer::Impl::Impl(xcb_input_device_id_t deviceID) {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();
   m_deviceID = deviceID;
}

void Pointer::SubscribeToWindow(const Window &window) {
   m_pImpl->SubscribeToWindow(
      window.m_pImpl->GetX11WindowID(), window.GetWindowID(),
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS |
                                  XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE | XCB_INPUT_XI_EVENT_MASK_MOTION |
                                  XCB_INPUT_XI_EVENT_MASK_ENTER | XCB_INPUT_XI_EVENT_MASK_LEAVE));

   // TODO: For reasons that are unknowable, raw input events refuse to work properly unless they are selected
   // for the root window only. So we have to register them seperately. The only mention of this I could find
   // anywhere is here: https://lists.freedesktop.org/archives/xorg/2020-May/060269.html
   // Look into if this will be a problem for multiple pointers, multiple windows, etc.
   m_pImpl->SubscribeToWindow(window.m_pImpl->GetX11RootWindowID(), window.GetWindowID(),
                              (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_RAW_MOTION));
}

bool Pointer::HasEvent() const noexcept {
   return m_pImpl->HasEvent();
}

Event Pointer::GetNextEvent() {
   return m_pImpl->GetNextEvent();
}