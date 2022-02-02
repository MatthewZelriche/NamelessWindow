#include "W32Window.hpp"

#include "Events/W32EventBus.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "W32DllMain.hpp"

using namespace NLSWIN;

std::shared_ptr<NLSWIN::Window> NLSWIN_API_PRIVATE NLSWIN::Window::Create() {
   std::shared_ptr<W32Window> impl = std::make_shared<W32Window>(WindowProperties());
   W32EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

std::shared_ptr<NLSWIN::Window> NLSWIN::Window::Create(WindowProperties properties) {
   std::shared_ptr<W32Window> impl = std::make_shared<W32Window>(properties);
   W32EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

W32Window::W32Window(WindowProperties properties) {
   win32Class.lpfnWndProc = &StaticWndProc;
   win32Class.hInstance = NLSWIN::GetDLLInstanceHandle();
   win32Class.lpszClassName = m_winClassName;
   if (!RegisterClass(&win32Class)) {
      throw PlatformInitializationException();
   }
   m_windowHandle =
      CreateWindowEx(0, m_winClassName, "Test window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                     CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, NLSWIN::GetDLLInstanceHandle(), this);
   if (!m_windowHandle) {
      throw PlatformInitializationException();
   }
}

void W32Window::Show() {
   // TODO: different Show options.
   ShowWindow(m_windowHandle, SW_SHOWNORMAL);
}

void W32Window::Hide() {
   ShowWindow(m_windowHandle, SW_HIDE);
}

void W32Window::DisableUserResizing() {
}
void W32Window::EnableUserResizing() {
}
void W32Window::SetFullscreen(bool borderless) noexcept {
}
void W32Window::SetWindowed() noexcept {
}
void W32Window::Reposition(uint32_t newX, uint32_t newY) noexcept {
}
void W32Window::Resize(uint32_t width, uint32_t height) noexcept {
}

LRESULT W32Window::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
   W32Window *windowInstance;

   switch (uMsg) {
      // When a new window is constructed, we want to set things up so we can
      // "redirect" events to the appropriate window instance so that the instance
      // can handle the event.
      case WM_NCCREATE: {
         // Get information on the newly created window.
         CREATESTRUCT *winInstCreateStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
         windowInstance = reinterpret_cast<W32Window *>(winInstCreateStruct->lpCreateParams);
         // Now that we have the newly created window, we can set that object's window handle.
         windowInstance->m_windowHandle = hWnd;
         // Associate our object instance with the window handle so that we can retrieve it
         // within this method at a later time.
         SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowInstance));
         break;
      }
      default: {
         windowInstance = reinterpret_cast<W32Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
         if (windowInstance) {
            // Don't forget to return here, we don't want to send the event both to the window
            // and back to the default procedure.
            return windowInstance->InstanceWndProc(uMsg, wParam, lParam);
         }
      }
   }

   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT W32Window::InstanceWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
   switch (uMsg) {
      case WM_CLOSE: {
         m_shouldClose = true;
         return 0;
      }
   }

   return DefWindowProc(m_windowHandle, uMsg, wParam, lParam);
}