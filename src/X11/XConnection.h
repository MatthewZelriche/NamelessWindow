#pragma once

#include <xcb/xcb.h>

#include "NamelessWindow/NLSAPI.h"

namespace NLSWIN {
class NLSWIN_API_PRIVATE XConnection {
   private:
   static xcb_connection_t* m_xServerConnection;

   public:
   static void CreateConnection();
   [[nodiscard]] inline static xcb_connection_t* GetConnection() noexcept { return m_xServerConnection; }
};
}  // namespace NLSWIN