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
class NLSWIN_API_PRIVATE W32Cursor : public Cursor, public W32InputDevice, public W32BaseMouse {
   public:
   W32Cursor();

   void BindToWindow(const Window *const window) noexcept override;
   void UnbindFromWindows() noexcept override;
   void ShowCursor() noexcept override;
   void HideCursor() noexcept override;
   void ProcessGenericEvent(MSG event) override;

   private:
   Event PackageButtonEvent(MSG event, ButtonValue value, ButtonPressType type, HWND window);

   WindowID m_inhabitedWindow {0};
};

}  // namespace NLSWIN