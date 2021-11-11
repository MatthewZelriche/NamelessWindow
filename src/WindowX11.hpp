#pragma once
#include <vector>

#include "NamelessWindow/Window.hpp"

class xcb_connection_t;
class xcb_screen_t;

namespace NLSWIN {
class NLSWIN_API_PRIVATE WindowX11 : public Window {
   private:
   xcb_connection_t *m_xServerConnection = nullptr;
   xcb_screen_t *GetScreenFromMonitor(Monitor monitor);

   public:
   WindowX11(WindowProperties properties);
   virtual int SetSomething(int value) override;
};
}  // namespace NLSWIN
