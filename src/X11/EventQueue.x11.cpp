#include "EventQueue.x11.hpp"

#include <X11/extensions/XInput2.h>
#include <xcb/xinput.h>

#include <cstring>

#include "NamelessWindow/Events/Key.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::vector<std::weak_ptr<EventListenerX11>> EventQueueX11::m_listeners;
xcb_connection_t *EventQueueX11::m_connection = nullptr;

void EventQueueX11::GetOSEvents() {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();
   xcb_generic_event_t *event = nullptr;
   while (event = xcb_poll_for_event(m_connection)) {
      for (auto listener: m_listeners) {
         if (!listener.expired()) {
            auto listenerSharedPtr = listener.lock();
            listenerSharedPtr->ProcessGenericEvent(event);
         }
      }
   }
}

void EventQueueX11::RegisterListener(std::weak_ptr<EventListenerX11> listener) {
   if (!listener.expired()) {
      m_listeners.push_back(listener);
   }
}

void EventQueue::GetOSEvents() {
   EventQueueX11::GetOSEvents();
}