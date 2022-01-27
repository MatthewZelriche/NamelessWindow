#pragma once

#include <unordered_map>
#include <utility>

#include "NamelessWindow/Events/Event.hpp"
#include "X11EventListener.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE X11GenericMouse {
   protected:
   float TranslateXCBFloat(xcb_input_fp1616_t inval) const noexcept;
   float TranslateXCBFloat(xcb_input_fp3232_t inval) const noexcept;
   std::pair<Event, Event> PackageNewDeltaEvents(xcb_input_raw_button_press_event_t *event);
   ButtonValue TranslateButton(uint16_t detail);

   private:
   xcb_window_t m_boundWindow {0};
   std::unordered_map<uint16_t, ButtonValue> m_buttonTranslationTable = {{1, ButtonValue::LEFTCLICK},
                                                                         {2, ButtonValue::MIDDLECLICK},
                                                                         {3, ButtonValue::RIGHTCLICK},
                                                                         {8, ButtonValue::MB_4},
                                                                         {9, ButtonValue::MB_5}};
};

}  // namespace NLSWIN