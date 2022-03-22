#include "W32Window.hpp"

#include "Events/W32EventBus.hpp"
#include "Events/W32EventThreadDispatcher.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "W32DllMain.hpp"
#include "W32Util.hpp"

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
   std::wstring className = ConvertToWString(m_winClassName);
   win32Class.lpszClassName = className.c_str();
   if (!RegisterClassW(&win32Class)) {
      throw PlatformInitializationException();
   }

   Win32CreationProps props {0};
   props.className = className;

   // Determine where to spawn the window.
   if (properties.preferredMonitor.has_value()) {
      props.X = properties.preferredMonitor.value().screenXCord;
      props.Y = properties.preferredMonitor.value().screenYCord;
   } else {
      props.X = CW_USEDEFAULT;
      props.Y = CW_USEDEFAULT;
   }
   props.nWidth = properties.horzResolution;
   props.nHeight = properties.vertResolution;

   // Determine whether this window is resizable. 
   props.dwStyle = WS_OVERLAPPEDWINDOW;
   if (!properties.isUserResizable) {
      props.dwStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
   }
   props.hInstance = win32Class.hInstance;
   if (!properties.windowName.empty()) {
      props.windowName = ConvertToWString(properties.windowName).c_str();
   }

   // TODO: Respect application choice of starting windowed or fullscreen.

   m_windowHandle = (HWND)SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), CREATE_NLSWIN_WINDOW,
                                       (WPARAM)&props, 0);
   if (!m_windowHandle) {
      throw PlatformInitializationException();
   }

   // Store size & pos, in case windows couldnt construct our desired size & pos.
   UpdateRectProperties();

   NewID();
}

void W32Window::UpdateRectProperties() {
   RECT rect;
   GetWindowRect(m_windowHandle, &rect);
   m_width = rect.left - rect.right;
   m_height = rect.top - rect.bottom;
   m_xPos = rect.left;
   m_yPos = rect.top;
}

void W32Window::Show() {
   // TODO: different Show options.
   ShowWindow(m_windowHandle, SW_SHOWNORMAL);
}

void W32Window::Hide() {
   ShowWindow(m_windowHandle, SW_HIDE);
}

void W32Window::DisableUserResizing() {
   SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX);
   SetWindowPos(m_windowHandle, 0, m_xPos, m_yPos, m_width, m_height, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER 
                                                                        | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
}
void W32Window::EnableUserResizing() {
   SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
   SetWindowPos(m_windowHandle, 0, m_xPos, m_yPos, m_width, m_height, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER 
                                                                        | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
}
void W32Window::SetFullscreen(bool borderless) noexcept {
}

void W32Window::SetWindowed() noexcept {
}

void W32Window::Reposition(uint32_t newX, uint32_t newY) noexcept {
   SetWindowPos(m_windowHandle, 0, newX, newY, m_width, m_height, SWP_FRAMECHANGED | SWP_NOOWNERZORDER 
                                                                        | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
   UpdateRectProperties();
}

void W32Window::Resize(uint32_t width, uint32_t height) noexcept {
   SetWindowPos(m_windowHandle, 0, m_xPos, m_yPos, width, height, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER 
                                                                        | SWP_NOZORDER | SWP_SHOWWINDOW);
   UpdateRectProperties();
}

void W32Window::ProcessGenericEvent(MSG event) {
   // Always remember to convert the WParam to our special type
   // @see WParamWithWindowHandle
   WParamWithWindowHandle* wParam = reinterpret_cast<WParamWithWindowHandle*>(event.wParam);
   if (wParam->sourceWindow == m_windowHandle) {
      switch (event.message) {
         case WM_CLOSE: {
            m_shouldClose = true;
            break;
         }
         case WM_SIZE: {
            m_width = LOWORD(event.lParam);
            m_height = HIWORD(event.lParam);
            WindowResizeEvent resizeEvent {0};
            resizeEvent.newWidth = m_width;
            resizeEvent.newHeight = m_height;
            resizeEvent.sourceWindow = GetGenericID();
            PushEvent(resizeEvent);
            break;
         }
         case WM_MOVE: {
            m_xPos = LOWORD(event.lParam);
            m_yPos = HIWORD(event.lParam);
            break;
         }
      }
   }
}

static BOOL CALLBACK MonitorEnumerationCallback(HMONITOR monitor, HDC deviceContext, RECT* rect, LPARAM lParam) {
   std::vector<MonitorInfo> *monitors = reinterpret_cast<std::vector<MonitorInfo> *>(lParam);
   MONITORINFOEX info;
   info.cbSize = sizeof(MONITORINFOEX);
   GetMonitorInfo(monitor, &info);

   // Calculate resolution from rect
   unsigned int xRes = abs(info.rcMonitor.left - info.rcMonitor.right);
   unsigned int yRes = abs(info.rcMonitor.top - info.rcMonitor.bottom);

   MonitorInfo monitorInfo {xRes, yRes, info.rcMonitor.left, info.rcMonitor.top, info.szDevice};
   monitors->push_back(monitorInfo);
   // Keep enumerating until there's nothing left.
   return true;
}


std::vector<MonitorInfo> NLSWIN::Window::EnumerateMonitors() {
   SetProcessDPIAware();
   std::vector<MonitorInfo> monitors;
   EnumDisplayMonitors(nullptr, nullptr, MonitorEnumerationCallback, (LPARAM)&monitors);
   return monitors;
}