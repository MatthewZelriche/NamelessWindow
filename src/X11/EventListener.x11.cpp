#include "EventListener.x11.hpp"

#include <stdexcept>

#include "EventQueue.x11.hpp"

using namespace NLSWIN;

bool EventListenerX11::HasEvent() const noexcept {
   return !m_Queue.empty();
}

void EventListenerX11::PushEvent(Event event) {
   m_Queue.push(event);
}

Event EventListenerX11::GetNextEvent() {
   if (!HasEvent()) {
      throw std::runtime_error("Attempted to get event from empty queue.");
   }
   Event test = std::move(m_Queue.front());
   m_Queue.pop();
   return test;
}