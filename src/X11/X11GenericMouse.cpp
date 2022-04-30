#include "X11GenericMouse.hpp"
#include <variant>

#include "X11InputDevice.hpp"
#include "X11Util.hpp"

using namespace NLSWIN;

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

Event X11GenericMouse::PackageNewDeltaEvents(xcb_input_raw_button_press_event_t *event) {
   xcb_input_raw_button_press_event_t *buttonEvent =
      reinterpret_cast<xcb_input_raw_button_press_event_t *>(event);

   // Ignore other raw motions, based on the valuator indices they contain. This is the best way
   // I can think of for disregarding scroll raw motion events. Indices 0 and 1 appear to be
   // real mouse motion events, while 2 and 3 appear to be horz/vertical valuators for scroll.
   auto mask = xcb_input_raw_button_press_valuator_mask(buttonEvent);
   if ((mask[0] & (1 << 2)) || (mask[0] & (1 << 3))) {
      return std::monostate();
   }
   auto rawAxisValues =
      xcb_input_raw_button_press_axisvalues_raw((xcb_input_raw_button_press_event_t *)event);
   RawMouseDeltaMovementEvent rawDeltaMoveEvent;
   rawDeltaMoveEvent.deltaX = TranslateXCBFloat(rawAxisValues[0]);
   rawDeltaMoveEvent.deltaY = TranslateXCBFloat(rawAxisValues[1]);

   return rawDeltaMoveEvent;
}