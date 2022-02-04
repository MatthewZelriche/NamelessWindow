#include "W32EventBus.hpp"

#include <windows.h>

#include "../W32DllMain.hpp"
#include "W32EventThreadDispatcher.hpp"

using namespace NLSWIN;

W32EventBus &W32EventBus::GetInstance() {
   static W32EventBus instance;
   return instance;
}

void W32EventBus::FreeOldEvents() {
   for (auto wParam: m_eventsToFreeNextPoll) { free(wParam); }
   m_eventsToFreeNextPoll.clear();
}

void W32EventBus::PollEvents() {
   // The WPARAMs we received last call  MUST be freed at the start of the next next call!
   FreeOldEvents();
   MSG event;
   while (PeekMessageA(&event, 0, 0, 0, PM_REMOVE)) {
      WParamWithWindowHandle *wParam = reinterpret_cast<WParamWithWindowHandle *>(event.wParam);
      m_eventsToFreeNextPoll.push_back(wParam);
      TranslateMessage(&event);
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