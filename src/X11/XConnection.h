#pragma once

#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

#include "NamelessWindow/NLSAPI.h"

namespace NLSWIN {
class NLSWIN_API_PRIVATE XConnection {
   private:
   static xcb_connection_t* m_xServerConnection;
   static Display* m_Display;

   public:
   static void CreateConnection();
   [[nodiscard]] inline static xcb_connection_t* GetConnection() noexcept { return m_xServerConnection; }
   [[nodiscard]] inline static Display* GetDisplay() noexcept { return m_Display; }
};
}  // namespace NLSWIN