#include "EventQueue.x11.hpp"

#include <cstring>

#include "NamelessWindow/exceptions.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::map<std::weak_ptr<EventListenerX11>, std::unordered_set<std::type_index>, std::owner_less<>>
   EventQueueX11::m_listeners;
xcb_connection_t *EventQueueX11::m_connection = nullptr;

void EventQueueX11::GetOSEvents() {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();

   xcb_generic_event_t *event = nullptr;
   while (event = xcb_poll_for_event(m_connection)) {
      switch (event->response_type & ~0x80) {
      case XCB_CLIENT_MESSAGE:
         xcb_client_message_event_t *clientEvent = reinterpret_cast<xcb_client_message_event_t *>(event);
         xcb_intern_atom_cookie_t deleteWindowCookie =
            xcb_intern_atom(m_connection, false, std::strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
         xcb_intern_atom_reply_t *deleteWindowAtomReply =
            xcb_intern_atom_reply(m_connection, deleteWindowCookie, nullptr);
         if (clientEvent->data.data32[0] == deleteWindowAtomReply->atom) {
            for (auto listener: m_listeners) {
               if (!listener.first.expired()) {
                  auto listenerSharedPtr = listener.first.lock();
                  auto &windows          = listenerSharedPtr->GetWindows();
                  if (windows.find(clientEvent->window) != windows.end()) {
                     listenerSharedPtr->EventRecieved(WindowCloseEvent());
                  }
               }
            }
         }
      }
   }
}

void EventQueueX11::RegisterXConnection(xcb_connection_t *connection) {
   if (m_connection) {
      return;
   }
   m_connection = connection;
}

void EventQueueX11::RegisterForEvent(std::weak_ptr<EventListenerX11> listener, std::type_index eventType) {
   // Attempt to add the listener to the map with a default-initialized vector as its key.
   // If it already exists, does nothing.
   auto &vectorRef = m_listeners[listener];
   vectorRef.insert(eventType);
}

bool EventQueueX11::UnregisterForEvent(std::weak_ptr<EventListenerX11> listener, std::type_index eventType) {
   if (m_listeners.find(listener) != m_listeners.end()) {
      auto &vectorRef = m_listeners[listener];
      if (vectorRef.erase(eventType) == 1) {
         return true;
      } else {
         return false;
      }
   } else {
      return false;
   }
}

void EventQueue::GetOSEvents() {
   EventQueueX11::GetOSEvents();
}