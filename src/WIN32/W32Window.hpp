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
#define OEMRESOURCE
#include <windows.h>
#include <WinUser.h>
// clang-format on
#include <unordered_map>

#include "Events/W32EventListener.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "NamelessWindow/Window.hpp"

struct Win32VidMode {
   int resX;
   int resY;
   int bits;
};

namespace NLSWIN {
/*!
 * @brief Represents a single Win32 window.
 * @ingroup WIN32
 *
 */
class NLSWIN_API_PRIVATE W32Window : public Window, public W32EventListener {
   public:
   W32Window(WindowProperties properties);
   ~W32Window();
   void Show() override;
   void Hide() override;
   void SetFullscreen() override;
   void SetWindowed() noexcept override;
   void Reposition(uint32_t newX, uint32_t newY) noexcept override;
   void Resize(uint32_t width, uint32_t height) override;
   void Focus() noexcept override;
   void EnableBorderless() noexcept override;
   void DisableBorderless() noexcept override;
   void Minimize(bool restoreVideoMode = false) override;
   [[nodiscard]] inline bool RequestedClose() const noexcept override { return m_shouldClose; }
   [[nodiscard]] inline Point GetWindowPos() const noexcept override { return {m_xPos, m_yPos}; }
   [[nodiscard]] inline WindowMode GetWindowMode() const noexcept override { return m_windowMode; }
   [[nodiscard]] unsigned int GetWindowWidth() const noexcept override { return m_width; }
   [[nodiscard]] unsigned int GetWindowHeight() const noexcept override { return m_height; }
   [[nodiscard]] bool IsBorderless() const noexcept override { return m_borderless; }
   [[nodiscard]] HDC GetDeviceContext() const noexcept { return m_deviceContext; }

   void ProcessGenericEvent(MSG event) override;
   [[nodiscard]] inline HWND GetWin32Handle() const noexcept { return m_windowHandle; }

   [[nodiscard]] static inline WindowID IDFromHWND(HWND handle) { return m_handleMap.at(handle); }

   private:
   void SetNewVideoMode(int width, int height, int bitsPerPixel);
   void UpdateRectProperties();
   void UpdateWindowData();
   std::pair<long, long> GetWindowSizeFromClientSize(int width, int height);
   int m_width {0};
   int m_height {0};
   int m_xPos {0};
   int m_yPos {0};
   WindowMode m_windowMode;
   bool m_shouldClose {false};
   bool m_borderless {false};
   bool m_minimized {false};

   std::wstring m_winClassName = L"NLSWINCLASS";
   HWND m_windowHandle {nullptr};
   WNDCLASSW win32Class {0};
   WNDPROC m_messageFuncPtr;
   HDC m_deviceContext {0};
   Win32VidMode m_cachedVideoMode;

   static std::unordered_map<HWND, WindowID> m_handleMap;

   int m_formatID {0};
   PIXELFORMATDESCRIPTOR pixelFormatDesc = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,
      32,  // On glx this is the addition of the res, green, blue, and alpha bit size = 32.
      8,
      0,
      8,
      0,
      8,
      0,
      8,
      0,
      0,
      0,
      0,
      0,
      0,
      24,
      8,
      0,
      0,
      0,
      0,
      0,
      0};
};
}  // namespace NLSWIN