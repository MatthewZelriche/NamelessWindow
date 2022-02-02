#include "W32EventBus.hpp"

using namespace NLSWIN;

W32EventBus &W32EventBus::GetInstance() {
   static W32EventBus instance;
   return instance;
}

void W32EventBus::PollEvents() {
   // TODO: Events...
}

void W32EventBus::RegisterListener(std::weak_ptr<W32EventListener> listener) {
   if (!listener.expired()) {
      m_listeners.push_back(listener);
   }
}

void EventBus::PollEvents() {
   W32EventBus::GetInstance().PollEvents();
}