#include "W32Window.hpp"

#include "Events/W32EventBus.hpp"
#include "Events/W32EventThreadDispatcher.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "W32DllMain.hpp"

using namespace NLSWIN;

std::shared_ptr<NLSWIN::Window> NLSWIN::Window::Create() {
   return Create(WindowProperties());
}

std::shared_ptr<NLSWIN::Window> NLSWIN::Window::Create(WindowProperties properties) {
   // See W32EventThreadDispatcher class for why this exists. It has to do with
   // win32 events being handled in a blocking manner.
   static bool isFirstWindow = true;
   if (isFirstWindow) {
      W32EventThreadDispatcher::Initialize();
      isFirstWindow = false;
   }
   std::shared_ptr<W32Window> impl = std::make_shared<W32Window>(properties);
   W32EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

W32Window::W32Window(WindowProperties properties) {
   win32Class.lpfnWndProc = &W32EventThreadDispatcher::DispatchProc;
   win32Class.hInstance = NLSWIN::GetDLLInstanceHandle();
   win32Class.lpszClassName = m_winClassName;
   if (!RegisterClass(&win32Class)) {
      throw PlatformInitializationException();
   }

   Win32CreationProps props {0};
   props.lpClassName = win32Class.lpszClassName;
   // TODO: Position
   props.X = CW_USEDEFAULT;
   props.Y = CW_USEDEFAULT;
   props.nWidth = properties.horzResolution;
   props.nHeight = properties.vertResolution;
   props.dwStyle = WS_OVERLAPPEDWINDOW;
   props.hInstance = win32Class.hInstance;
   if (!properties.windowName.empty()) {
      props.lpWindowName = properties.windowName.c_str();
   }
   m_windowHandle = (HWND)SendMessageA(W32EventThreadDispatcher::GetDispatcherHandle(), CREATE_NLSWIN_WINDOW,
                                       (WPARAM)&props, 0);
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