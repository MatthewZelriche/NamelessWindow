#include "EventListener.x11.hpp"

#include "EventQueue.x11.hpp"

using namespace NLSWIN;

bool EventListenerX11::HasEvent() {
   return !m_Queue.empty();
}

void EventListenerX11::EventRecieved(Event event) {
   m_Queue.push(event);
}

Event EventListenerX11::GetNextEvent() {
   Event test = std::move(m_Queue.front());
   m_Queue.pop();
   return test;
}