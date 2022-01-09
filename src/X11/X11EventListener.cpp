#include "X11EventListener.hpp"

#include "NamelessWindow/Exceptions.hpp"

using namespace NLSWIN;

bool X11EventListener::HasEvent() const noexcept {
   return !m_Queue.empty();
}

void X11EventListener::PushEvent(Event event) {
   m_Queue.push(event);
}

Event X11EventListener::GetNextEvent() {
   if (!HasEvent()) {
      throw EmptyEventQueueException();
   }
   Event test = std::move(m_Queue.front());
   m_Queue.pop();
   return test;
}

void X11EventListener::SubscribeToEvents(xcb_event_mask_t mask) {
   m_subscribedEventMask = mask;
}

void X11EventListener::SubscribeToXInputEvents(xcb_input_xi_event_mask_t mask) {
   m_subscribedXInputMask = mask;
}