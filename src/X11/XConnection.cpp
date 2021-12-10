#include "XConnection.h"

#include "NamelessWindow/Exceptions.hpp"

using namespace NLSWIN;

xcb_connection_t* XConnection::m_xServerConnection = nullptr;

void XConnection::CreateConnection() {
   if (!m_xServerConnection) {
      m_xServerConnection = xcb_connect(nullptr, nullptr);
      int result = xcb_connection_has_error(m_xServerConnection);
      if (result != 0) {
         throw PlatformInitializationException();
      }
   }
}