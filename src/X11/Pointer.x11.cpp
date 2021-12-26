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
               MouseButtonEvent mouseButtonEvent;
               mouseButtonEvent.button = X11InputMapper::TranslateButton(buttonPressEvent->detail);
               mouseButtonEvent.type = ButtonPressType::PRESSED;
               mouseButtonEvent.xPos = TranslateXCBFloat(buttonPressEvent->event_x);
               mouseButtonEvent.yPos = (buttonPressEvent->event_y);
               PushEvent(mouseButtonEvent);
            }
         }
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
               mouseButtonEvent.yPos = (buttonReleaseEvent->event_y);
               PushEvent(mouseButtonEvent);
            }
         }
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
   m_subscribedMasks =
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS |
                                  XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE | XCB_INPUT_XI_EVENT_MASK_MOTION);
   m_deviceID = deviceID;
}

void Pointer::SubscribeToWindow(const Window &window) {
   m_pImpl->SubscribeToWindow(window.m_pImpl->GetX11WindowID(), window.GetWindowID());
}

bool Pointer::HasEvent() const noexcept {
   return m_pImpl->HasEvent();
}

Event Pointer::GetNextEvent() {
   return m_pImpl->GetNextEvent();
}