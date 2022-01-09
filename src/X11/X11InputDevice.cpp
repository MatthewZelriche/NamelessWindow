#include "X11InputDevice.hpp"

#include "XConnection.h"

using namespace NLSWIN;

X11InputDevice::X11InputDevice(xcb_input_device_id_t id) noexcept {
   m_deviceID = id;
}

void X11InputDevice::ProcessGenericEvent(xcb_generic_event_t *event) {
   switch (event->response_type & ~0x80) {
      // We SHOULD only ever get XCB_GE_GENERIC from the EventBus, but just in case, check and assert
      // otherwise.
      case XCB_GE_GENERIC: {
         xcb_ge_generic_event_t *genericEvent = reinterpret_cast<xcb_ge_generic_event_t *>(event);
         ProcessXInputEvent(genericEvent);
      }
      default:
         assert(false);
   }
}

void X11InputDevice::SubscribeToWindow(xcb_window_t x11Handle, WindowID windowID) {
   m_subscribedWindowX11Handles.insert(x11Handle);
   m_subscribedWindowGenericIDs.insert(windowID);

   XI2EventMask mask;
   mask.head.deviceid = m_deviceID;
   mask.head.mask_len = sizeof(mask.mask) / sizeof(uint32_t);
   mask.mask = GetSubscribedXInputEventTypes();
   auto cookie = xcb_input_xi_select_events_checked(XConnection::GetConnection(), x11Handle, 1, &mask.head);
   xcb_flush(XConnection::GetConnection());
}

void X11InputDevice::UnsubscribeFromWindow(xcb_window_t x11Handle, WindowID windowID) {
   m_subscribedWindowX11Handles.erase(x11Handle);
   m_subscribedWindowGenericIDs.erase(windowID);

   XI2EventMask mask;
   mask.head.deviceid = m_deviceID;
   mask.head.mask_len = 0;  // Length of zero clears the mask on the X server,
   auto cookie = xcb_input_xi_select_events_checked(XConnection::GetConnection(), x11Handle, 1, &mask.head);
   xcb_flush(XConnection::GetConnection());
}