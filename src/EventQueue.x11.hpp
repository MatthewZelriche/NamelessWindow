#pragma once

#include <map>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_set>
#include <vector>

#include "EventListener.x11.hpp"
#include "NamelessWindow/Event.hpp"
#include "NamelessWindow/EventQueue.hpp"

namespace NLSWIN {

class EventQueueX11 {
   private:
   static std::map<std::weak_ptr<EventListenerX11>, std::unordered_set<std::type_index>, std::owner_less<>>
      m_listeners;
   static xcb_connection_t *m_connection;

   public:
   static void RegisterXConnection(xcb_connection_t *connection);
   static void GetOSEvents();
   static void RegisterForEvent(std::weak_ptr<EventListenerX11> listener, std::type_index eventType);
   static bool UnregisterForEvent(std::weak_ptr<EventListenerX11> listener, std::type_index eventType);
};

}  // namespace NLSWIN