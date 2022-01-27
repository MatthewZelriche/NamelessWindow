#include "X11RawMouse.hpp"

#include "X11EventBus.hpp"
#include "X11Window.hpp"

using namespace NLSWIN;

std::shared_ptr<RawMouse> RawMouse::Create(MouseDeviceInfo device) {
   std::shared_ptr<X11RawMouse> impl = std::make_shared<X11RawMouse>(device);
   X11EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

X11RawMouse::X11RawMouse(MouseDeviceInfo device) {
   m_deviceID = device.platformSpecificIdentifier;
   SubscribeToRawRootEvents(m_inputEventMask);
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

void X11RawMouse::ProcessXInputEvent(xcb_ge_generic_event_t *event) {
   xcb_ge_generic_event_t *inputEvent = reinterpret_cast<xcb_ge_generic_event_t *>(event);
   switch (inputEvent->event_type) {
      case XCB_INPUT_BUTTON_PRESS: {
         xcb_input_button_press_event_t *buttonPressEvent =
            reinterpret_cast<xcb_input_button_press_event_t *>(inputEvent);
         if (buttonPressEvent->deviceid == m_deviceID) {
            PushEvent(PackageNewRawButtonPressEvent(buttonPressEvent));
         }
         break;
      }
      case XCB_INPUT_BUTTON_RELEASE: {
         xcb_input_button_release_event_t *buttonReleaseEvent =
            reinterpret_cast<xcb_input_button_release_event_t *>(inputEvent);
         if (buttonReleaseEvent->deviceid == m_deviceID) {
            PushEvent(PackageNewRawButtonReleaseEvent(buttonReleaseEvent));
         }
         break;
      }
      case XCB_INPUT_RAW_MOTION: {
         xcb_input_raw_motion_event_t *rawEvent =
            reinterpret_cast<xcb_input_raw_motion_event_t *>(inputEvent);
         if (rawEvent->deviceid == m_deviceID) {
            auto deltaEvents = PackageNewDeltaEvents(rawEvent);
            PushEvent(deltaEvents.first);
            PushEvent(deltaEvents.second);
         }
         break;
      }
   }
}