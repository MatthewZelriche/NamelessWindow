#include "EventDispatcher.x11.hpp"

#include <X11/extensions/XInput2.h>
#include <xcb/xinput.h>

#include <cstring>

#include "NamelessWindow/Events/Key.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::vector<std::weak_ptr<EventListenerX11>> EventDispatcherX11::m_listeners;
xcb_connection_t *EventDispatcherX11::m_connection = nullptr;
std::vector<xcb_generic_event_t *> EventDispatcherX11::m_eventsToFreeNextPoll;

void EventDispatcherX11::GetOSEvents() {
   m_connection = XConnection::GetConnection();
   FreeOldEvents();
   xcb_generic_event_t *event = nullptr;
   while (event = xcb_poll_for_event(m_connection)) {
      m_eventsToFreeNextPoll.push_back(event);
      for (auto iter = m_listeners.begin(); iter != m_listeners.end();) {
         if (!(*iter).expired()) {
            auto listenerSharedPtr = (*iter).lock();
            listenerSharedPtr->ProcessGenericEvent(event);
            iter++;
         } else {
            // Erase expired listeners - no longer needed.
            iter = m_listeners.erase(iter);
         }
      }
   }
}

void EventDispatcherX11::FreeOldEvents() {
   for (auto event: m_eventsToFreeNextPoll) { free(event); }
   m_eventsToFreeNextPoll.clear();
}

void EventDispatcherX11::RegisterListener(std::weak_ptr<EventListenerX11> listener) {
   if (!listener.expired()) {
      m_listeners.push_back(listener);
   }
}

void EventDispatcher::GetOSEvents() {
   EventDispatcherX11::GetOSEvents();
}