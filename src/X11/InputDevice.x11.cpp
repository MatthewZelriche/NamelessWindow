#include "InputDevice.x11.hpp"

#include "NamelessWindow/Window.hpp"

using namespace NLSWIN;

void InputDeviceX11::ProcessGenericEvent(xcb_generic_event_t *event) {
   switch (event->response_type & ~0x80) {
      // Send only Xinput2 events.
      case XCB_GE_GENERIC: {
         xcb_ge_generic_event_t *genericEvent = reinterpret_cast<xcb_ge_generic_event_t *>(event);
         ProcessXInputEvent(genericEvent);
      }
   }
}

void InputDeviceX11::SubscribeToWindow(xcb_window_t x11Handle, WindowID windowID) {
   m_SubscribedWindows.insert({x11Handle, windowID});

   XI2EventMask mask;
   mask.head.deviceid = m_deviceID;
   mask.head.mask_len = sizeof(mask.mask) / sizeof(uint32_t);
   mask.mask = m_subscribedMasks;
   xcb_input_xi_select_events(m_connection, x11Handle, 1, &mask.head);
   xcb_flush(m_connection);  // To ensure the X server definitely gets the request.
}