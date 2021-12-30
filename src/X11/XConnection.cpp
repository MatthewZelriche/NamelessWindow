#include "XConnection.h"

#include "NamelessWindow/Exceptions.hpp"
#include "xcb/xfixes.h"

using namespace NLSWIN;

xcb_connection_t* XConnection::m_xServerConnection = nullptr;
Display* XConnection::m_Display = nullptr;

void XConnection::CreateConnection() {
   if (!m_xServerConnection) {
      m_Display = XOpenDisplay(NULL);
      m_xServerConnection = XGetXCBConnection(m_Display);
      auto cookie =
         xcb_xfixes_query_version(m_xServerConnection, XCB_XFIXES_MAJOR_VERSION, XCB_XFIXES_MINOR_VERSION);
      auto reply = xcb_xfixes_query_version_reply(m_xServerConnection, cookie, nullptr);
      // 4.0 or higher is needed for cursor visibility functions.
      if (reply->major_version < 4) {
         throw PlatformInitializationException();
      }
   }
}