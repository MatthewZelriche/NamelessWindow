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
      win32Class.hCursor = (HCURSOR)LoadImageW(nullptr, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, 0, 0,
                                               LR_SHARED | LR_DEFAULTSIZE);
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

   auto newSizes = GetWindowSizeFromClientSize(props.nWidth, props.nHeight);
   props.nWidth = newSizes.first;
   props.nHeight = newSizes.second;
   m_windowHandle = (HWND)SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), CREATE_NLSWIN_WINDOW,
                                       (WPARAM)&props, 0);
   UpdateRectProperties();

   if (!m_windowHandle) {
      throw PlatformInitializationException();
   }

   if (properties.startBorderless) {
      EnableBorderless();
   }

   if (properties.mode == WindowMode::FULLSCREEN) {
      // Temporarily set our width and height to the desired, to avoid issues with setting video mode.
      // They will later be updated to the correct values at the end of the constructor.
      m_width = properties.horzResolution;
      m_height = properties.vertResolution;
      Show();
      SetFullscreen();
      Hide();
   } else {
      m_windowMode = WindowMode::WINDOWED;
   }

   // Set up pixel format for OGL.
   // TODO: Lock guard around this?
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
   POINT clientPoint = {rect.left, rect.top};
   ClientToScreen(m_windowHandle, &clientPoint);
   m_xPos = clientPoint.x;
   m_yPos = clientPoint.y;
}

void W32Window::Show() {
   // TODO: different Show options.
   ShowWindow(m_windowHandle, SW_SHOWNORMAL);
}

void W32Window::Hide() {
   ShowWindow(m_windowHandle, SW_HIDE);
}

void W32Window::UpdateWindowData() {
   SetWindowPos(m_windowHandle, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void W32Window::EnableBorderless() noexcept {
   if (m_windowMode == NLSWIN::WindowMode::FULLSCREEN) {
      return;
   }
   SetWindowLongPtrW(m_windowHandle, GWL_STYLE,
                     GetWindowLongW(m_windowHandle, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME));
   SetWindowPos(m_windowHandle, 0, m_xPos, m_yPos, m_width, m_height,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW);
   m_borderless = true;
   // Perform a custom reposition - otherwise windows will attempt to place the top-left of the
   // client area at the top-left of the window decorations.
   Reposition(m_xPos, m_yPos);
   UpdateRectProperties();
}

void W32Window::DisableBorderless() noexcept {
   if (m_windowMode == NLSWIN::WindowMode::FULLSCREEN) {
      return;
   }
   SetWindowLongPtrW(m_windowHandle, GWL_STYLE,
                     GetWindowLongW(m_windowHandle, GWL_STYLE) | WS_CAPTION | WS_THICKFRAME);
   auto newSize = GetWindowSizeFromClientSize(m_width, m_height);
   SetWindowPos(m_windowHandle, 0, m_xPos, m_yPos, newSize.first, newSize.second,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW);
   m_borderless = false;
   Reposition(m_xPos, m_yPos);
   UpdateRectProperties();
}

void W32Window::SetFullscreen() {
   if (m_windowMode == WindowMode::FULLSCREEN) {
      return;
   }
   HMONITOR monitor = MonitorFromWindow(m_windowHandle, MONITOR_DEFAULTTONEAREST);
   MONITORINFO info;
   info.cbSize = sizeof(MONITORINFO);
   GetMonitorInfo(monitor, &info);

   SetNewVideoMode(m_width, m_height, 32);

   // Temporary removal of window decorations, will be reverted upon the next call to SetWindowed()
   SetWindowLong(m_windowHandle, GWL_STYLE,
                 GetWindowLong(m_windowHandle, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME));
   UpdateWindowData();

   // Get the topleft corner of the monitor we are fullscreening on.
   int resX = abs(info.rcMonitor.left - info.rcMonitor.right);
   int resY = abs(info.rcMonitor.top - info.rcMonitor.bottom);

   Reposition(info.rcMonitor.left, info.rcMonitor.top);
   Resize(resX, resY);

   m_windowMode = WindowMode::FULLSCREEN;
   UpdateRectProperties();
}

void W32Window::SetWindowed() noexcept {
   if (m_windowMode == WindowMode::WINDOWED) {
      return;
   }

   // Revert display settings to those stored in registry.
   ChangeDisplaySettingsW(nullptr, 0);

   // Restore window decorations, if needed.
   if (!m_borderless) {
      SetWindowLongPtr(m_windowHandle, GWL_STYLE,
                       GetWindowLong(m_windowHandle, GWL_STYLE) | WS_CAPTION | WS_THICKFRAME);
      UpdateWindowData();
   }
   m_windowMode = NLSWIN::WindowMode::WINDOWED;
   Resize(m_width, m_height);
   Reposition(m_xPos, m_yPos);

   UpdateRectProperties();
}

std::pair<long, long> W32Window::GetWindowSizeFromClientSize(int width, int height) {
   RECT desiredClientSize;
   desiredClientSize.left = m_xPos;
   desiredClientSize.top = m_yPos;
   desiredClientSize.right = m_xPos + width;
   desiredClientSize.bottom = m_yPos + height;
   AdjustWindowRect(&desiredClientSize, GetWindowLong(m_windowHandle, GWL_STYLE) | WS_CAPTION, false);
   long adjustedWidth = desiredClientSize.right - desiredClientSize.left;
   long adjustedHeight = desiredClientSize.bottom - desiredClientSize.top;

   return {adjustedWidth, adjustedHeight};
}

void W32Window::Reposition(uint32_t newX, uint32_t newY) noexcept {
   if (!m_borderless) {
      TITLEBARINFO info;
      info.cbSize = sizeof(TITLEBARINFO);
      GetTitleBarInfo(m_windowHandle, &info);
      int titleBarHeight = info.rcTitleBar.bottom - info.rcTitleBar.top;

      RECT windowArea;
      GetWindowRect(m_windowHandle, &windowArea);
      // Assuming all four borders are the same thickness - could be incorrect for weird themes.
      int borderThickness = ((windowArea.right - windowArea.left) - m_width) / 2;
      SetWindowPos(m_windowHandle, 0, newX - borderThickness, newY - titleBarHeight - borderThickness, 0, 0,
                   SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
   } else {
      SetWindowPos(m_windowHandle, 0, newX, newY, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
   }
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

void W32Window::Resize(uint32_t width, uint32_t height) {
   if (m_windowMode == WindowMode::FULLSCREEN) {
      SetNewVideoMode(width, height, 32);
   }

   // Check if we currently have border decorations or not.
   // SetWindowPos seems to always assume decorations exist, even when we have them off.
   if (GetWindowLongW(m_windowHandle, GWL_STYLE) & (WS_CAPTION | WS_THICKFRAME)) {
      auto adjustedSize = GetWindowSizeFromClientSize(width, height);
      SetWindowPos(m_windowHandle, 0, m_xPos, m_yPos, adjustedSize.first, adjustedSize.second, SWP_NOMOVE);
   } else {
      SetWindowPos(m_windowHandle, 0, m_xPos, m_yPos, width, height, SWP_NOMOVE);
   }

   UpdateRectProperties();
}

void W32Window::Focus() noexcept {
   // SetFocus has to be called on the thread that the window was created on.
   // So we must pass this message to our message thread.
   SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), USER_FOCUS_WINDOW, (WPARAM)m_windowHandle,
                0);
}

void W32Window::Minimize(bool restoreVideoMode) {
   if (restoreVideoMode) {
      ChangeDisplaySettingsW(nullptr, 0);
      m_cachedVideoMode = Win32VidMode {m_width, m_height, 32};
   }
   ShowWindow(m_windowHandle, SW_MINIMIZE);
   m_minimized = true;
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
               if ((m_windowMode == WindowMode::FULLSCREEN) && m_minimized) {
                  SetNewVideoMode(m_cachedVideoMode.resX, m_cachedVideoMode.resY, m_cachedVideoMode.bits);
               }
               m_minimized = false;
            }
            break;
         }
         case WM_KILLFOCUS: {
            if (wParam->sourceWindow == m_windowHandle) {
               PushEvent(WindowFocusLostEvent {GetGenericID()});
            }
            break;
         }
         case WM_SYSCOMMAND: {
            if (wParam->sourceWindow == m_windowHandle) {
                if (wParam->wParam == SC_MINIMIZE) {
                  m_minimized = true;
               }
            }
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