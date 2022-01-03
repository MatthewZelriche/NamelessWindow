#pragma once

#include <memory>
#include <vector>

#include "EventListener.x11.hpp"
#include "NamelessWindow/Events/EventDispatcher.hpp"
#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE EventDispatcherX11 {
   private:
   static std::vector<std::weak_ptr<EventListenerX11>> m_listeners;
   static xcb_connection_t *m_connection;
   static std::vector<xcb_generic_event_t *> m_eventsToFreeNextPoll;
   static void FreeOldEvents();

   public:
   static void GetOSEvents();
   static void RegisterListener(std::weak_ptr<EventListenerX11> listener);
};

}  // namespace NLSWIN