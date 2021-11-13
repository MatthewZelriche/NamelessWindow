#pragma once
#include <xcb/xcb.h>

#include <vector>

#include "NamelessWindow/Window.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE WindowX11 : public Window {
   private:
   static xcb_connection_t *m_xServerConnection;
   int m_preferredScreenNum   = 0;
   xcb_window_t m_x11WindowID = 0;

   xcb_screen_t *GetScreenFromMonitor(Monitor monitor);
   void ToggleFullscreen();

   public:
   WindowX11(WindowProperties properties);
   virtual void SetFullscreen(bool borderless) override;
   virtual void SetWindowed() override;
};
}  // namespace NLSWIN
