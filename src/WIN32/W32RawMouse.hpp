/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup WIN32 Windows API
 * @brief Platform-specific Windows implementation of the API
 */
#pragma once

#include <array>
#include <unordered_map>

#include "Events/W32EventListener.hpp"
#include "NamelessWindow/RawMouse.hpp"
#include "W32BaseMouse.hpp"
#include "W32Window.hpp"

namespace NLSWIN {

/*! @ingroup WIN32 */
class NLSWIN_API_PRIVATE W32RawMouse : public RawMouse, public W32EventListener, public W32BaseMouse {
   public:
   W32RawMouse(MouseDeviceInfo device);
   ButtonValue TranslateButton(unsigned int win32ButtonID);

   private:
   void ProcessGenericEvent(MSG event) override;
   Event PackageButtonEvent(RAWMOUSE event);
   std::array<bool, 5> m_rawButtonState {false};

   uint64_t m_deviceSpecifier {0};

   static bool s_firstInit;
   static std::shared_ptr<W32Window> s_rawInputHandle;
   std::unordered_map<uint16_t, ButtonValue> m_buttonTranslationTable = {
      {RI_MOUSE_LEFT_BUTTON_DOWN, ButtonValue::LEFTCLICK},
      {RI_MOUSE_LEFT_BUTTON_UP, ButtonValue::LEFTCLICK},
      {RI_MOUSE_RIGHT_BUTTON_DOWN, ButtonValue::RIGHTCLICK},
      {RI_MOUSE_RIGHT_BUTTON_UP, ButtonValue::RIGHTCLICK},
      {RI_MOUSE_MIDDLE_BUTTON_DOWN, ButtonValue::MIDDLECLICK},
      {RI_MOUSE_MIDDLE_BUTTON_UP, ButtonValue::MIDDLECLICK},
      {RI_MOUSE_BUTTON_4_DOWN, ButtonValue::MB_4},
      {RI_MOUSE_BUTTON_4_UP, ButtonValue::MB_4},
      {RI_MOUSE_BUTTON_5_DOWN, ButtonValue::MB_5},
      {RI_MOUSE_BUTTON_5_UP, ButtonValue::MB_5}};
};

}  // namespace NLSWIN