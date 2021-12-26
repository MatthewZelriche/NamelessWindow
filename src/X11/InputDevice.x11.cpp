#include "InputDevice.x11.hpp"

#include "NamelessWindow/Exceptions.hpp"
#include "NamelessWindow/Window.hpp"
#include "X11/extensions/XInput2.h"
#include "XConnection.h"

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

void InputDeviceX11::SubscribeToWindow(xcb_window_t x11Handle, WindowID windowID,
                                       xcb_input_xi_event_mask_t masks) {
   m_SubscribedWindows.insert({x11Handle, windowID});

   XI2EventMask mask;
   mask.head.deviceid = m_deviceID;
   mask.head.mask_len = sizeof(mask.mask) / sizeof(uint32_t);
   mask.mask = masks;
   auto cookan = xcb_input_xi_select_events_checked(m_connection, x11Handle, 1, &mask.head);
   auto error = xcb_request_check(m_connection, cookan);
   if (error) {
      throw BadEventRegistrationException();
   }
   xcb_flush(m_connection);  // To ensure the X server definitely gets the request.
}