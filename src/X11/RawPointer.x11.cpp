#include "RawPointer.x11.hpp"

#include <cstring>

#include "EventQueue.x11.hpp"
#include "NamelessWindow/Window.hpp"
#include "Window.x11.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::vector<PointerDeviceInfo> RawPointer::EnumeratePointers() noexcept {
   return EnumerateDevicesX11<PointerDeviceInfo>(XCB_INPUT_DEVICE_TYPE_SLAVE_POINTER);
}

RawPointer::Impl::Impl(xcb_input_device_id_t deviceID, const Window &window) : PointerDeviceX11(deviceID) {
   m_deviceID = deviceID;
   SubscribeToWindow(
      window.m_pImpl->GetX11WindowID(), window.m_pImpl->GetWindowID(),
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS |
                                  XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE | XCB_INPUT_XI_EVENT_MASK_ENTER |
                                  XCB_INPUT_XI_EVENT_MASK_LEAVE | XCB_INPUT_XI_EVENT_MASK_MOTION));
}

RawPointer::RawPointer(PointerDeviceInfo device, const Window &window) :
   m_pImpl(std::make_shared<RawPointer::Impl>(device.platformSpecificIdentifier, window)) {
   EventQueueX11::RegisterListener(m_pImpl);
}

void RawPointer::Impl::ProcessXInputEvent(xcb_ge_generic_event_t *event) {
   switch (event->event_type) {
      case XCB_INPUT_BUTTON_PRESS: {
         xcb_input_button_press_event_t *buttonPressEvent =
            reinterpret_cast<xcb_input_button_press_event_t *>(event);
         if (buttonPressEvent->deviceid == m_deviceID) {
            PackageButtonPressEvent(buttonPressEvent);
         }
         break;
      }
      case XCB_INPUT_BUTTON_RELEASE: {
         xcb_input_button_release_event_t *buttonReleaseEvent =
            reinterpret_cast<xcb_input_button_release_event_t *>(event);
         if (buttonReleaseEvent->deviceid == m_deviceID) {
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

RawPointer::~RawPointer() {
}

bool RawPointer::HasEvent() const noexcept {
   return m_pImpl->HasEvent();
}

Event RawPointer::GetNextEvent() {
   return m_pImpl->GetNextEvent();
}