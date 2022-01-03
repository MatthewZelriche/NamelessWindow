#include "RawPointer.x11.hpp"

#include <cstring>

#include "EventDispatcher.x11.hpp"
#include "NamelessWindow/Window.hpp"
#include "Window.x11.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::vector<PointerDeviceInfo> Pointer::EnumeratePointers() noexcept {
   return EnumerateDevicesX11<PointerDeviceInfo>(XCB_INPUT_DEVICE_TYPE_SLAVE_POINTER);
}

RawPointerX11::RawPointerX11(xcb_input_device_id_t deviceID, const Window *const window) :
   PointerDeviceX11(deviceID) {
   m_deviceID = deviceID;
   auto windowImpl = static_cast<const WindowX11 *const>(window);
   BindToWindow(window);

   SubscribeToWindow(
      windowImpl->GetX11WindowID(), windowImpl->GetWindowID(),
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS |
                                  XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE | XCB_INPUT_XI_EVENT_MASK_ENTER |
                                  XCB_INPUT_XI_EVENT_MASK_LEAVE | XCB_INPUT_XI_EVENT_MASK_MOTION));
}

std::shared_ptr<Pointer> Pointer::Create(PointerDeviceInfo device, const Window *const window) {
   std::shared_ptr<RawPointerX11> impl =
      std::make_shared<RawPointerX11>(device.platformSpecificIdentifier, window);
   EventDispatcherX11::RegisterListener(impl);
   return std::move(impl);
}

void RawPointerX11::BindToWindow(const Window *const window) noexcept {
   m_boundWindow = static_cast<const WindowX11 *const>(window)->GetX11WindowID();
   if (m_subscribedWindows.count(m_boundWindow) > 0) {
      return;
   }
   xcb_set_input_focus(m_connection, 0, m_boundWindow, XCB_CURRENT_TIME);
   m_subscribedWindows.clear();
   SubscribeToWindow(
      m_boundWindow, window->GetWindowID(),
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS |
                                  XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE | XCB_INPUT_XI_EVENT_MASK_ENTER |
                                  XCB_INPUT_XI_EVENT_MASK_LEAVE | XCB_INPUT_XI_EVENT_MASK_MOTION));
}

void RawPointerX11::ProcessXInputEvent(xcb_ge_generic_event_t *event) {
   switch (event->event_type) {
      case XCB_INPUT_BUTTON_PRESS: {
         xcb_input_button_press_event_t *buttonPressEvent =
            reinterpret_cast<xcb_input_button_press_event_t *>(event);
         if (buttonPressEvent->deviceid == m_deviceID && m_boundWindow == buttonPressEvent->event) {
            PackageButtonPressEvent(buttonPressEvent);
         }
         break;
      }
      case XCB_INPUT_BUTTON_RELEASE: {
         xcb_input_button_release_event_t *buttonReleaseEvent =
            reinterpret_cast<xcb_input_button_release_event_t *>(event);
         if (buttonReleaseEvent->deviceid == m_deviceID && m_boundWindow == buttonReleaseEvent->event) {
            PackageButtonReleaseEvent(buttonReleaseEvent);
         }
         break;
      }
      case XCB_INPUT_RAW_MOTION: {
         xcb_input_raw_motion_event_t *rawEvent = reinterpret_cast<xcb_input_raw_motion_event_t *>(event);
         if (rawEvent->deviceid == m_deviceID) {
            PackageDeltaEvent(rawEvent);
         }
         break;
      }
   }
}