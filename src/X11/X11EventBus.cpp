#include "X11EventBus.hpp"

#include <xcb/xcb.h>

#include "XConnection.h"

using namespace NLSWIN;

void X11EventBus::PollEvents() {
   // xcb events are dynamically allocated, so to avoid memory leaks we must free all events from the last
   // poll.
   FreeOldEvents();
   xcb_generic_event_t *event = nullptr;
   while (event = xcb_poll_for_event(XConnection::GetConnection())) {
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

void X11EventBus::FreeOldEvents() {
   for (auto event: m_eventsToFreeNextPoll) { free(event); }
   m_eventsToFreeNextPoll.clear();
}

void X11EventBus::RegisterListener(std::weak_ptr<X11EventListener> listener) {
   if (!listener.expired()) {
      m_listeners.push_back(listener);
   }
}

X11EventBus &X11EventBus::GetInstance() {
   static X11EventBus instance;
   return instance;
}

void EventBus::PollEvents() {
   X11EventBus::GetInstance().PollEvents();
}