#include "W32EventListener.hpp"

#include "NamelessWindow/Exceptions.hpp"

using namespace NLSWIN;

bool NLSWIN::W32EventListener::HasEvent() const noexcept {
   return !m_Queue.empty();
}
NLSWIN::Event NLSWIN::W32EventListener::GetNextEvent() {
   if (!HasEvent()) {
      throw EmptyEventQueueException();
   }
   Event test = std::move(m_Queue.front());
   m_Queue.pop();
   return test;
}

void NLSWIN::W32EventListener::PushEvent(Event event) {
   m_Queue.push(event);
}