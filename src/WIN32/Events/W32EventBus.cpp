#include "W32EventBus.hpp"

#include <windows.h>

using namespace NLSWIN;

W32EventBus &W32EventBus::GetInstance() {
   static W32EventBus instance;
   return instance;
}

void W32EventBus::PollEvents() {
   MSG event;
   while (PeekMessage(&event, nullptr, 0, 0, PM_REMOVE)) {
      // TODO: Redirect input events to input classes...Assuming thats possible.

      TranslateMessage(&event);
      // For some reason WindowProcedure methods won't get some unqueued events unless DispatchMessage is
      // being called, so we must always call it.
      DispatchMessage(&event);
   }
}

void W32EventBus::RegisterListener(std::weak_ptr<W32EventListener> listener) {
   if (!listener.expired()) {
      m_listeners.push_back(listener);
   }
}

void EventBus::PollEvents() {
   W32EventBus::GetInstance().PollEvents();
}