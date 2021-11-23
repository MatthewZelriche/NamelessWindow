#pragma once
#include <xcb/xcb.h>

#include <vector>

#include "NamelessWindow/Window.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE Window::WindowImpl {
   private:
   const Window &self;
   static xcb_connection_t *m_xServerConnection;
   int m_preferredScreenNum {0};
   xcb_window_t m_x11WindowID {0};

   xcb_screen_t *GetScreenFromMonitor(Monitor monitor);
   void ToggleFullscreen();
   WindowMode m_currentWindowMode;

   public:
   WindowImpl(WindowProperties properties, const Window &window);
   void SetFullscreen(bool borderless);
   void SetWindowed();

   inline WindowMode GetWindowMode() const { return m_currentWindowMode; }
};
}  // namespace NLSWIN
