#include "Keyboard.x11.hpp"

#include <xcb/xinput.h>

#include <cstring>

#include "EventQueue.x11.hpp"
#include "NamelessWindow/Event.hpp"
#include "NamelessWindow/Window.hpp"
#include "Window.x11.hpp"
#include "XConnection.h"

using namespace NLSWIN;

xcb_connection_t *Keyboard::KeyboardImpl::m_connection = nullptr;

std::vector<KeyboardDeviceInfo> Keyboard::EnumerateKeyboards() {
   std::vector<KeyboardDeviceInfo> keyboards;
   // Open a brief temporary connection to get the screens
   xcb_connection_t *connection = xcb_connect(nullptr, nullptr);
   int result                   = xcb_connection_has_error(connection);
   if (result != 0) {
      xcb_disconnect(connection);
      return {};
   }

   xcb_input_xi_query_device_cookie_t queryCookie =
      xcb_input_xi_query_device(connection, XCB_INPUT_DEVICE_ALL);
   xcb_input_xi_query_device_reply_t *reply =
      xcb_input_xi_query_device_reply(connection, queryCookie, nullptr);

   xcb_input_xi_device_info_iterator_t iter = xcb_input_xi_query_device_infos_iterator(reply);
   while (iter.rem > 0) {
      auto element = iter.data;
      if (element->enabled) {
         if (element->type == XCB_INPUT_DEVICE_TYPE_SLAVE_KEYBOARD) {
            // Ignore xtest devices
            const char *name = xcb_input_xi_device_info_name(element);
            if (!std::strstr(name, "XTEST")) {
               KeyboardDeviceInfo dev {};
               dev.name                       = name;
               dev.platformSpecificIdentifier = element->deviceid;
               keyboards.push_back(dev);
            }
         }
      }
      xcb_input_xi_device_info_next(&iter);
   }
   free(reply);
   return keyboards;
}

Keyboard::KeyboardImpl::KeyboardImpl(const Window &window) {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();

   m_windows.insert(window.m_pImpl->GetWindowID());
}

Keyboard::Keyboard(const Window &window) : m_pImpl(std::make_shared<Keyboard::KeyboardImpl>(window)) {
   // After weve constructed the impl, register all events it is interested in.
   EventQueueX11::RegisterForEvent(m_pImpl, KeyEvent::type);
}

bool Keyboard::HasEvent() {
   return m_pImpl->HasEvent();
}

Event Keyboard::GetNextEvent() {
   return m_pImpl->GetNextEvent();
}

Keyboard::~Keyboard() {
}