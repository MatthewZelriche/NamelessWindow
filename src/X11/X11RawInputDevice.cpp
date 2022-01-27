#include "X11RawInputDevice.hpp"

#include "X11Util.hpp"

using namespace NLSWIN;

void X11RawInputDevice::SubscribeToRawRootEvents(xcb_input_xi_event_mask_t masks) {
   UTIL::XI2EventMask mask;
   mask.header.deviceid = m_deviceID;
   mask.header.mask_len = sizeof(mask.mask) / sizeof(uint32_t);
   mask.mask = masks;
   auto cookie = xcb_input_xi_select_events_checked(XConnection::GetConnection(), UTIL::GetRootWindow(), 1,
                                                    &mask.header);
   xcb_flush(XConnection::GetConnection());  // To ensure the X server definitely gets the request.
}

void X11RawInputDevice::SubscribeToXInputEvents(xcb_input_xi_event_mask_t mask) {
   m_subscribedXInputMask = mask;
}

void X11RawInputDevice::ProcessGenericEvent(xcb_generic_event_t *event) {
   switch (event->response_type & ~0x80) {
      case XCB_GE_GENERIC: {
         xcb_ge_generic_event_t *genericEvent = reinterpret_cast<xcb_ge_generic_event_t *>(event);
         ProcessXInputEvent(genericEvent);
      }
   }
}