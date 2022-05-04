#include "W32Window.hpp"

#include "Events/W32EventBus.hpp"
#include "Events/W32EventThreadDispatcher.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "W32DllMain.hpp"
#include "W32Util.hpp"

using namespace NLSWIN;

std::unordered_map<HWND, WindowID> W32Window::m_handleMap;

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
   // Only register this new window class once.
   static bool isFirstWindow = true;
   if (isFirstWindow) {
      win32Class.lpfnWndProc = &W32EventThreadDispatcher::DispatchProc;
      win32Class.hInstance = NLSWIN::GetDLLInstanceHandle();
      win32Class.lpszClassName = m_winClassName.c_str();
      win32Class.style = CS_OWNDC;
      if (!RegisterClassW(&win32Class)) {
         throw PlatformInitializationException();
      }
      isFirstWindow = false;
   }

   Win32CreationProps props {0};
   props.className = m_winClassName;

   // Determine where to spawn the window.
   props.X = properties.xCoordinate;
   props.Y = properties.yCoordinate;
   if (properties.preferredMonitor.has_value()) {
      props.X += properties.preferredMonitor.value().screenXCord;
      props.Y += properties.preferredMonitor.value().screenYCord;
   }
   props.nWidth = properties.horzResolution;
   props.nHeight = properties.vertResolution;

   props.dwStyle = WS_OVERLAPPEDWINDOW;
   props.hInstance = win32Class.hInstance;
   if (!properties.windowName.empty()) {
      props.windowName = ConvertToWString(properties.windowName).c_str();
   }

   m_windowHandle = (HWND)SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), CREATE_NLSWIN_WINDOW,
                                       (WPARAM)&props, 0);
   if (!m_windowHandle) {
      throw PlatformInitializationException();
   }
   // Determine whether this window is resizable.
   if (!properties.isUserResizable) {
      SetWindowLongPtr(m_windowHandle, GWL_STYLE, props.dwStyle & ~(WS_BORDER | WS_THICKFRAME | WS_MAXIMIZEBOX));
   }

   Reposition(props.X, props.Y);

   if (properties.mode == WindowMode::FULLSCREEN) {
      Show();
      SetFullscreen(false);
      Hide();
   } else if (properties.mode == WindowMode::BORDERLESS) {
      Show();
      SetFullscreen(true);
      Hide();
   }

   // Set up pixel format for OGL.
   m_deviceContext = GetDC(m_windowHandle);
   m_formatID = ChoosePixelFormat(m_deviceContext, &pixelFormatDesc);
   SetPixelFormat(m_deviceContext, m_formatID, &pixelFormatDesc);

   Reposition(props.X, props.Y);

   // Store size & pos, in case windows couldnt construct our desired size & pos.
   UpdateRectProperties();

   NewID();
   m_handleMap.insert({m_windowHandle, GetGenericID()});
}

W32Window::~W32Window() {
   SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), DESTROY_NLSWIN_WINDOW,
                (WPARAM)m_windowHandle, 0);
   m_handleMap.erase(m_windowHandle);
}

void W32Window::UpdateRectProperties() {
   RECT rect;
   GetClientRect(m_windowHandle, &rect);
   m_width = rect.right - rect.left;
   m_height = rect.bottom - rect.top;
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
   m_userResizable = false;
   SetWindowLongPtr(m_windowHandle, GWL_STYLE,
                    GetWindowLongPtr(m_windowHandle, GWL_STYLE) & ~(WS_BORDER | WS_THICKFRAME | WS_MAXIMIZEBOX));
   SetWindowPos(
      m_windowHandle, 0, m_xPos, m_yPos, m_width, m_height,
      SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

void W32Window::EnableUserResizing() {
   m_userResizable = true;
   SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
   SetWindowPos(
      m_windowHandle, 0, m_xPos, m_yPos, m_width, m_height,
      SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

void W32Window::SetFullscreen(bool borderless) noexcept {
   if (m_windowMode == WindowMode::WINDOWED) {
      return;
   } else if (m_windowMode == WindowMode::FULLSCREEN && !borderless) {
      return;
   } else if (m_windowMode == WindowMode::BORDERLESS && borderless) {
      return;
   }

   HMONITOR monitor = MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST);
   MONITORINFO info;
   info.cbSize = sizeof(MONITORINFO);
   GetMonitorInfo(monitor, &info);
   SetWindowLong(
      m_windowHandle, GWL_STYLE,
      GetWindowLong(m_windowHandle, GWL_STYLE) & WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
   int resX = abs(info.rcMonitor.left - info.rcMonitor.right);
   int resY = abs(info.rcMonitor.top - info.rcMonitor.bottom);
   SetWindowPos(m_windowHandle, 0, info.rcMonitor.left, info.rcMonitor.top, resX, resY,
                SWP_FRAMECHANGED | SWP_SHOWWINDOW);

   if (borderless) {
      m_windowMode = WindowMode::BORDERLESS;
   } else {
      m_windowMode = WindowMode::FULLSCREEN;
   }
   UpdateRectProperties();
}

void W32Window::SetWindowed() noexcept {
   if (m_windowMode == WindowMode::WINDOWED) {
      return;
   }

   HMONITOR monitor = MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST);
   MONITORINFO info;
   info.cbSize = sizeof(MONITORINFO);
   GetMonitorInfo(monitor, &info);

   // Set correct window style.
   if (m_userResizable) {
      SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX);
   } else {
      SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
   }
   int resX = abs(info.rcMonitor.left - info.rcMonitor.right);
   int resY = abs(info.rcMonitor.top - info.rcMonitor.bottom);
   auto adjustedSize = GetWindowSizeFromClientSize(resX, resY);
   SetWindowPos(m_windowHandle, 0, info.rcMonitor.left, info.rcMonitor.top, adjustedSize.first, adjustedSize.second,
                SWP_FRAMECHANGED | SWP_SHOWWINDOW);
   UpdateRectProperties();
}

std::pair<long, long> W32Window::GetWindowSizeFromClientSize(int width, int height) {
   RECT desiredClientSize;
   desiredClientSize.left = m_xPos;
   desiredClientSize.top = m_yPos;
   desiredClientSize.right = m_xPos + width;
   desiredClientSize.bottom = m_yPos + height;
   AdjustWindowRect(&desiredClientSize, GetWindowLong(m_windowHandle, GWL_STYLE), false);
   long adjustedWidth = desiredClientSize.right - desiredClientSize.left;
   long adjustedHeight = desiredClientSize.bottom - desiredClientSize.top;

   return {adjustedWidth, adjustedHeight};
}

void W32Window::Reposition(uint32_t newX, uint32_t newY) noexcept {
   SetWindowPos(m_windowHandle, 0, newX, newY, m_width, m_height,
                SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
   UpdateRectProperties();
}

void W32Window::SetNewVideoMode(int width, int height, int bitsPerPixel) {
   // Get the monitor name we should change the video mode for.
   HMONITOR monitor = MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST);
   MONITORINFO info;
   info.cbSize = sizeof(MONITORINFO);
   MONITORINFOEX infoWithName;
   infoWithName.cbSize = sizeof(MONITORINFOEX);
   GetMonitorInfo(monitor, &infoWithName);

   // Request a new devmode.
   DEVMODE mode {};
   mode.dmSize = sizeof(mode);
   mode.dmPelsWidth = width;
   mode.dmPelsHeight = height;
   mode.dmBitsPerPel = bitsPerPixel;
   mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
   int returnCode =
      ChangeDisplaySettingsEx(infoWithName.szDevice, &mode, nullptr, CDS_TEST | CDS_FULLSCREEN, nullptr);
   if (returnCode != DISP_CHANGE_SUCCESSFUL) {
      throw NLSWIN::InvalidVideoModeException();
   }
   // No issue, we can set new mode.
   ChangeDisplaySettingsEx(infoWithName.szDevice, &mode, nullptr, CDS_FULLSCREEN, nullptr);
}

void W32Window::Resize(uint32_t width, uint32_t height) noexcept {
   auto adjustedSize = GetWindowSizeFromClientSize(width, height);
   SetWindowPos(m_windowHandle, 0, m_xPos, m_yPos, adjustedSize.first, adjustedSize.second,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW);

   if (m_windowMode == WindowMode::FULLSCREEN) {
      SetNewVideoMode(width, height, 32);
   }

   UpdateRectProperties();
}

void W32Window::Focus() noexcept {
   // SetFocus has to be called on the thread that the window was created on.
   // So we must pass this message to our message thread.
   SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), USER_FOCUS_WINDOW, (WPARAM)m_windowHandle,
                0);
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
            WindowRepositionEvent repositionEvent;
            repositionEvent.newX = m_xPos;
            repositionEvent.newY = m_yPos;
            repositionEvent.sourceWindow = GetGenericID();
            PushEvent(repositionEvent);
            break;
         }
         case WM_SETFOCUS: {
            if (wParam->sourceWindow == m_windowHandle) {
               PushEvent(WindowFocusedEvent {GetGenericID()});
            }
            break;
         }
      }
   }
}

static BOOL CALLBACK MonitorEnumerationCallback(HMONITOR monitor, HDC deviceContext, RECT* rect,
                                                LPARAM lParam) {
   std::vector<MonitorInfo>* monitors = reinterpret_cast<std::vector<MonitorInfo>*>(lParam);
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