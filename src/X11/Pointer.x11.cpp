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

void Pointer::Impl::Init(xcb_input_device_id_t deviceID) {
   m_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
   xkb_x11_setup_xkb_extension(m_connection, XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION,
                               XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, nullptr, nullptr);
   m_deviceID = deviceID;
}

void Pointer::Impl::SubscribeToWindow(const Window &window) {
   xcb_window_t windowID = window.m_pImpl->GetX11WindowID();
   m_SubscribedWindows.insert({windowID, window.m_pImpl->GetWindowID()});

   XI2EventMask mask;
   mask.head.deviceid = m_deviceID;
   mask.head.mask_len = sizeof(mask.mask) / sizeof(uint32_t);
   mask.mask = m_subscribedMasks;
   xcb_input_xi_select_events(m_connection, windowID, 1, &mask.head);
   xcb_flush(m_connection);  // To ensure the X server definitely gets the request.
}

void Pointer::Impl::ProcessGenericEvent(xcb_generic_event_t *event) {
   switch (event->response_type & ~0x80) {
         // Handle Xinput2 events.
      case XCB_GE_GENERIC: {
         xcb_ge_generic_event_t *genericEvent = reinterpret_cast<xcb_ge_generic_event_t *>(event);
         switch (genericEvent->event_type) {
            case XCB_INPUT_BUTTON_PRESS: {
               xcb_input_button_press_event_t *buttonPressEvent =
                  reinterpret_cast<xcb_input_button_press_event_t *>(genericEvent);
               if (m_SubscribedWindows.count(buttonPressEvent->event)) {
                  if (GetDeviceID() == buttonPressEvent->deviceid ||
                      GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
                     MouseButtonEvent mouseButtonEvent {
                        X11InputMapper::TranslateButton(buttonPressEvent->detail), ButtonPressType::PRESSED,
                        TranslateXCBFloat(buttonPressEvent->event_x),
                        TranslateXCBFloat(buttonPressEvent->event_y)};
                     PushEvent(mouseButtonEvent);
                  }
               }
            }
            case XCB_INPUT_BUTTON_RELEASE: {
               xcb_input_button_release_event_t *buttonreleaseEvent =
                  reinterpret_cast<xcb_input_button_release_event_t *>(genericEvent);
               if (m_SubscribedWindows.count(buttonreleaseEvent->event)) {
                  if (GetDeviceID() == buttonreleaseEvent->deviceid ||
                      GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
                     MouseButtonEvent mouseButtonEvent {
                        X11InputMapper::TranslateButton(buttonreleaseEvent->detail),
                        ButtonPressType::RELEASED, TranslateXCBFloat(buttonreleaseEvent->event_x),
                        TranslateXCBFloat(buttonreleaseEvent->event_y)};
                     PushEvent(mouseButtonEvent);
                  }
               }
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

Pointer::Impl::Impl() {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();
   Init(2);
}

Pointer::Impl::Impl(PointerDeviceInfo device) {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();
   Init(device.platformSpecificIdentifier);
}

void Pointer::SubscribeToWindow(const Window &window) {
   m_pImpl->SubscribeToWindow(window);
}

bool Pointer::HasEvent() const noexcept {
   return m_pImpl->HasEvent();
}

Event Pointer::GetNextEvent() {
   return m_pImpl->GetNextEvent();
}