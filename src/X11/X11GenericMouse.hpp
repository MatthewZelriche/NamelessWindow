#pragma once

#include <unordered_map>

#include "X11EventListener.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE X11GenericMouse : public X11EventListener {
   public:
   X11GenericMouse(xcb_input_device_id_t device);

   protected:
   void SubscribeToRawRootEvents(xcb_input_xi_event_mask_t masks);
   inline xcb_input_device_id_t GetDeviceID() const noexcept { return m_deviceID; }
   float TranslateXCBFloat(xcb_input_fp1616_t inval) const noexcept;
   float TranslateXCBFloat(xcb_input_fp3232_t inval) const noexcept;
   void PushNewDeltaEvents(xcb_input_raw_button_press_event_t *event);
   ButtonValue TranslateButton(uint16_t detail);

   private:
   xcb_input_device_id_t m_deviceID {0};
   xcb_window_t m_boundWindow {0};
   std::unordered_map<uint16_t, ButtonValue> m_buttonTranslationTable = {{1, ButtonValue::LEFTCLICK},
                                                                         {2, ButtonValue::MIDDLECLICK},
                                                                         {3, ButtonValue::RIGHTCLICK},
                                                                         {8, ButtonValue::MB_4},
                                                                         {9, ButtonValue::MB_5}};
};

}  // namespace NLSWIN