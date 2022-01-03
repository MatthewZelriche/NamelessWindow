#pragma once

#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE XConnection {
   private:
   static xcb_connection_t* m_xServerConnection;
   static Display* m_Display;
   static void CreateConnection();
   XConnection();

   public:
   static xcb_connection_t* GetConnection() noexcept;
   static Display* GetDisplay() noexcept;
};
}  // namespace NLSWIN