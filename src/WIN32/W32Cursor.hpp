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

#include "NamelessWindow/Cursor.hpp"
#include "NamelessWindow/NLSAPI.hpp"

#include "Events/W32EventListener.hpp"
#include "W32InputDevice.hpp"
#include "W32BaseMouse.hpp"

namespace NLSWIN {
/*! @ingroup WIN32 */
class NLSWIN_API_PRIVATE W32Cursor : public Cursor, public W32EventListener, public W32BaseMouse {
   public:
   W32Cursor() = default;

   void Confine(Window* window) noexcept override;
   void Free() noexcept override;
   void Show() noexcept override;
   void Hide() noexcept override;
   void ProcessGenericEvent(MSG event) override;

   private:
   Event PackageButtonEvent(MSG event, ButtonValue value, ButtonPressType type, HWND window);
   void ConfineCursorToRect(HWND handle, RECT rect);

   bool m_blockCursorClip {false};
   bool m_beginClickOnNCArea {false};
   bool m_hideCursor {false};
   WindowID m_inhabitedWindow {0};
   WindowID m_focusedWindow {0};
   std::pair<WindowID, HWND> m_boundWindow {0, 0};
};

}  // namespace NLSWIN