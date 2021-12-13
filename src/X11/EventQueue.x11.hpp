#pragma once

#include <map>
#include <memory>
#include <typeindex>
#include <unordered_set>

#include "EventListener.x11.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventQueue.hpp"
#include "NamelessWindow/NLSAPI.h"

namespace NLSWIN {

class NLSWIN_API_PRIVATE EventQueueX11 {
   private:
   static std::vector<std::weak_ptr<EventListenerX11>> m_listeners;
   static xcb_connection_t *m_connection;

   public:
   static void GetOSEvents();
   static void RegisterListener(std::weak_ptr<EventListenerX11> listener);
};

}  // namespace NLSWIN