/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup X11 Linux X11 API
 * @brief Platform-specific X11 implementation of the API
 */
#pragma once

#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN {
/*! @ingroup X11 */
class NLSWIN_API_PRIVATE XConnection {
   public:
   static xcb_connection_t* GetConnection() noexcept;
   static Display* GetDisplay() noexcept;

   private:
   static xcb_connection_t* m_xServerConnection;
   static Display* m_Display;
   static void CreateConnection();
   XConnection();
   XConnection(XConnection const&);
   void operator=(XConnection const&);
};
}  // namespace NLSWIN