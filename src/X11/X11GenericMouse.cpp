#include "X11GenericMouse.hpp"

#include "X11InputDevice.hpp"
#include "X11Util.hpp"

using namespace NLSWIN;

X11GenericMouse::X11GenericMouse(xcb_input_device_id_t device) {
   m_deviceID = device;
}

void X11GenericMouse::SubscribeToRawRootEvents(xcb_input_xi_event_mask_t masks) {
   UTIL::XI2EventMask mask;
   mask.header.deviceid = m_deviceID;
   mask.header.mask_len = sizeof(mask.mask) / sizeof(uint32_t);
   mask.mask = masks;
   auto cookie = xcb_input_xi_select_events_checked(XConnection::GetConnection(), UTIL::GetRootWindow(), 1,
                                                    &mask.header);
   auto error = xcb_request_check(XConnection::GetConnection(), cookie);
   SubscribeToXInputEvents(masks);           // Don't forget to inform the base class that we've
                                             // subscribed to these events.
   xcb_flush(XConnection::GetConnection());  // To ensure the X server definitely gets the request.
}

float X11GenericMouse::TranslateXCBFloat(xcb_input_fp1616_t inval) const noexcept {
   return inval / (float)UINT16_MAX;
}

float X11GenericMouse::TranslateXCBFloat(xcb_input_fp3232_t inval) const noexcept {
   return inval.integral + inval.frac / (float)UINT32_MAX;
}

ButtonValue X11GenericMouse::TranslateButton(uint16_t detail) {
   if (m_buttonTranslationTable.find(detail) == m_buttonTranslationTable.end()) {
      return ButtonValue::NULLCLICK;
   }
   return m_buttonTranslationTable.at(detail);
}

void X11GenericMouse::PushNewDeltaEvents(xcb_input_raw_button_press_event_t *event) {
   xcb_input_raw_button_press_event_t *buttonEvent =
      reinterpret_cast<xcb_input_raw_button_press_event_t *>(event);

   // Ignore other raw motions, based on the valuator indices they contain. This is the best way
   // I can think of for disregarding scroll raw motion events. Indices 0 and 1 appear to be
   // real mouse motion events, while 2 and 3 appear to be horz/vertical valuators for scroll.
   auto mask = xcb_input_raw_button_press_valuator_mask(buttonEvent);
   if ((mask[0] & (1 << 2)) || (mask[0] & (1 << 3))) {
      return;
   }
   auto rawAxisValues =
      xcb_input_raw_button_press_axisvalues_raw((xcb_input_raw_button_press_event_t *)event);
   RawMouseDeltaMovementEvent rawDeltaMoveEvent;
   rawDeltaMoveEvent.deltaX = TranslateXCBFloat(rawAxisValues[0]);
   rawDeltaMoveEvent.deltaY = TranslateXCBFloat(rawAxisValues[1]);
   PushEvent(rawDeltaMoveEvent);
   //  Non-raw axisvalues gives us the accelerated delta.
   auto axisValues = xcb_input_raw_button_press_axisvalues((xcb_input_raw_button_press_event_t *)event);
   MouseDeltaMovementEvent deltaMoveEvent;
   deltaMoveEvent.deltaX = TranslateXCBFloat(axisValues[0]);
   deltaMoveEvent.deltaY = TranslateXCBFloat(axisValues[1]);

   PushEvent(deltaMoveEvent);
}