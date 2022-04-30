#include "X11RawMouse.hpp"
#include <xcb/xinput.h>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/RawMouse.hpp"
#include "X11EventBus.hpp"
#include "X11Window.hpp"

using namespace NLSWIN;

[[nodiscard]] std::vector<MouseDeviceInfo> RawMouse::EnumeratePointers() noexcept {
   return EnumerateDevicesX11<MouseDeviceInfo>(XCB_INPUT_DEVICE_TYPE_SLAVE_POINTER);
}

std::shared_ptr<RawMouse> RawMouse::Create(MouseDeviceInfo device) {
   std::shared_ptr<X11RawMouse> impl = std::make_shared<X11RawMouse>(device);
   X11EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

X11RawMouse::X11RawMouse(MouseDeviceInfo device) {
   m_deviceID = device.platformSpecificIdentifier;
   SubscribeToRawRootEvents(m_rawInputEventMask);
}

Event X11RawMouse::PackageNewRawButtonPressEvent(xcb_input_button_press_event_t *event) {
   if (event->detail >= 4 && event->detail <= 7) {
      RawMouseScrollEvent scrollEvent;
      scrollEvent.scrollType = (ScrollType)(event->detail - 4);
      return scrollEvent;
   }
   RawMouseButtonEvent mouseButtonEvent;
   mouseButtonEvent.button = TranslateButton(event->detail);
   mouseButtonEvent.type = ButtonPressType::PRESSED;
   return mouseButtonEvent;
}

Event X11RawMouse::PackageNewRawButtonReleaseEvent(xcb_input_button_press_event_t *event) {
   RawMouseButtonEvent mouseButtonEvent;
   // Only process scroll events on a button press - processing both on press and release gives
   // erroneous events.
   if (event->detail >= 4 && event->detail <= 7) {
      return std::monostate();
   }
   mouseButtonEvent.button = TranslateButton(event->detail);
   mouseButtonEvent.type = ButtonPressType::RELEASED;
   return mouseButtonEvent;
}

void X11RawMouse::ProcessGenericEvent(xcb_generic_event_t *event) {
   if ((event->response_type & ~0x80) != XCB_GE_GENERIC) {
      return;
   }
   xcb_ge_generic_event_t *genericEvent = reinterpret_cast<xcb_ge_generic_event_t *>(event);
   switch (genericEvent->event_type) {
      case XCB_INPUT_RAW_BUTTON_PRESS: {
         xcb_input_button_press_event_t *buttonPressEvent =
            reinterpret_cast<xcb_input_button_press_event_t *>(genericEvent);
         if (buttonPressEvent->deviceid == m_deviceID) {
            PushEvent(PackageNewRawButtonPressEvent(buttonPressEvent));
         }
         break;
      }
      case XCB_INPUT_RAW_BUTTON_RELEASE: {
         xcb_input_button_release_event_t *buttonReleaseEvent =
            reinterpret_cast<xcb_input_button_release_event_t *>(genericEvent);
         if (buttonReleaseEvent->deviceid == m_deviceID) {
            PushEvent(PackageNewRawButtonReleaseEvent(buttonReleaseEvent));
         }
         break;
      }
      case XCB_INPUT_RAW_MOTION: {
         xcb_input_raw_motion_event_t *rawEvent =
            reinterpret_cast<xcb_input_raw_motion_event_t *>(genericEvent);
         if (rawEvent->deviceid == m_deviceID) {
            PushEvent(PackageNewDeltaEvents(rawEvent));
         }
         break;
      }
   }
}