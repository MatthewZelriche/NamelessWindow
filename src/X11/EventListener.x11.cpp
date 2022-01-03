#include "EventListener.x11.hpp"

#include <stdexcept>

#include "EventDispatcher.x11.hpp"
#include "NamelessWindow/Exceptions.hpp"

using namespace NLSWIN;

bool EventListenerX11::HasEvent() const noexcept {
   return !m_Queue.empty();
}

void EventListenerX11::PushEvent(Event event) {
   m_Queue.push(event);
}

Event EventListenerX11::GetNextEvent() {
   if (!HasEvent()) {
      throw EmptyEventQueueException();
   }
   Event test = std::move(m_Queue.front());
   m_Queue.pop();
   return test;
}