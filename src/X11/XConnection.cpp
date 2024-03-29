#include "XConnection.h"

#include <xcb/xcb.h>
#include <xcb/xfixes.h>
#define explicit explicit_
#include <xcb/xkb.h>
#undef explicit
#include <xkbcommon/xkbcommon-x11.h>

#include "NamelessWindow/Exceptions.hpp"

using namespace NLSWIN;

xcb_connection_t* XConnection::m_xServerConnection = nullptr;
Display* XConnection::m_Display = nullptr;
uint8_t XConnection::m_xkbBaseEvent = 0;

void XConnection::CreateConnection() {
   if (!m_xServerConnection) {
      m_Display = XOpenDisplay(NULL);
      m_xServerConnection = XGetXCBConnection(m_Display);
      XSetEventQueueOwner(m_Display, XCBOwnsEventQueue);
      xkb_x11_setup_xkb_extension(m_xServerConnection, XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION,
                                  XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, &m_xkbBaseEvent,
                                  nullptr);
      auto cookie =
         xcb_xfixes_query_version(m_xServerConnection, XCB_XFIXES_MAJOR_VERSION, XCB_XFIXES_MINOR_VERSION);
      auto reply = xcb_xfixes_query_version_reply(m_xServerConnection, cookie, nullptr);
      // 4.0 or higher is needed for cursor visibility functions.
      // TODO: Test for XRandR version 5.0 and xcb-icccm and xinput version
      if (reply->major_version < 4) {
         free(reply);
         throw PlatformInitializationException();
      }
      free(reply);
   }
}

xcb_connection_t* XConnection::GetConnection() noexcept {
   if (!m_xServerConnection) {
      CreateConnection();
   }
   return m_xServerConnection;
}

Display* XConnection::GetDisplay() noexcept {
   if (!m_Display) {
      CreateConnection();
   }
   return m_Display;
}