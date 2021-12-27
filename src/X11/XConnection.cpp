#include "XConnection.h"

#include "NamelessWindow/Exceptions.hpp"

using namespace NLSWIN;

xcb_connection_t* XConnection::m_xServerConnection = nullptr;
Display* XConnection::m_Display = nullptr;

void XConnection::CreateConnection() {
   if (!m_xServerConnection) {
      m_Display = XOpenDisplay(NULL);
      m_xServerConnection = XGetXCBConnection(m_Display);
   }
}