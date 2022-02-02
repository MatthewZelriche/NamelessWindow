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
// clang-format off
#include <windows.h>
#include <WinUser.h>
// clang-format on

#include "Events/W32EventListener.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "NamelessWindow/Window.hpp"

namespace NLSWIN {
/*!
 * @brief Represents a single Win32 window.
 * @ingroup WIN32
 *
 */
class NLSWIN_API_PRIVATE W32Window : public Window, public W32EventListener {
   public:
   W32Window(WindowProperties properties);
   void Show() override;
   void Hide() override;
   void DisableUserResizing() override;
   void EnableUserResizing() override;
   void SetFullscreen(bool borderless = true) noexcept override;
   void SetWindowed() noexcept override;
   void Reposition(uint32_t newX, uint32_t newY) noexcept override;
   void Resize(uint32_t width, uint32_t height) noexcept override;
   [[nodiscard]] inline bool RequestedClose() const noexcept override { return m_shouldClose; }
   [[nodiscard]] inline WindowMode GetWindowMode() const noexcept override { return m_windowMode; }
   [[nodiscard]] unsigned int GetWindowWidth() const noexcept override { return m_width; }
   [[nodiscard]] unsigned int GetWindowHeight() const noexcept override { return m_height; }

   private:
   /*!
    * @brief Static method responsible for dispatching events to the appropriate window instance.
    *
    * @param hWnd The window handle that generated the event.
    * @param uMsg The type of the message.
    * @param wParam Message data.
    * @param lParam Message data.
    */
   static LRESULT StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
   /*!
    * @brief Method responsible for handling events associated with this window instance.
    *
    * @param uMsg The type of message.
    * @param wParam Message data.
    * @param lParam Message data.
    * @return LRESULT
    */
   LRESULT InstanceWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
   int m_width {0};
   int m_height {0};
   WindowMode m_windowMode;
   bool m_shouldClose {false};

   const char *m_winClassName = "NLSWINCLASS";
   HWND m_windowHandle {nullptr};
   WNDCLASS win32Class {0};
   WNDPROC m_messageFuncPtr;
};
}  // namespace NLSWIN