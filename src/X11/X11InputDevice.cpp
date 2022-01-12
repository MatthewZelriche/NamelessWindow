#include "X11InputDevice.hpp"

#include "X11Util.hpp"
#include "X11Window.hpp"

using namespace NLSWIN;

void X11InputDevice::ProcessGenericEvent(xcb_generic_event_t *event) {
   switch (event->response_type & ~0x80) {
      // TODO: Why are we getting non GE events?
      case XCB_GE_GENERIC: {
         xcb_ge_generic_event_t *genericEvent = reinterpret_cast<xcb_ge_generic_event_t *>(event);
         ProcessXInputEvent(genericEvent);
      }
   }
}

void X11InputDevice::SubscribeToWindow(const Window *const window) {
   auto x11Window = static_cast<const X11Window *const>(window);
   m_subscribedWindows.insert({x11Window->GetX11ID(), x11Window->GetGenericID()});

   UTIL::XI2EventMask mask;
   mask.header.deviceid = m_deviceID;
   mask.header.mask_len = sizeof(mask.mask) / sizeof(uint32_t);
   mask.mask = GetSubscribedXInputEventTypes();
   xcb_input_xi_select_events_checked(XConnection::GetConnection(), x11Window->GetX11ID(), 1, &mask.header);
   xcb_flush(XConnection::GetConnection());
}

void X11InputDevice::UnsubscribeFromWindow(const Window *const window) {
   auto x11Window = static_cast<const X11Window *const>(window);
   m_subscribedWindows.erase(x11Window->GetX11ID());

   UTIL::XI2EventMask mask;
   mask.header.deviceid = m_deviceID;
   mask.header.mask_len = 0;  // Length of zero clears the mask on the X server,
   xcb_input_xi_select_events_checked(XConnection::GetConnection(), x11Window->GetX11ID(), 1, &mask.header);
   xcb_flush(XConnection::GetConnection());
}