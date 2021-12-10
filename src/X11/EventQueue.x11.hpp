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
   static std::map<std::weak_ptr<EventListenerX11>, std::unordered_set<EventType>, std::owner_less<>>
      m_listeners;
   static xcb_connection_t *m_connection;

   static void DispatchEventToListeners(Event event, xcb_window_t windowID, EventType eventType,
                                        xcb_input_device_id_t deviceID = 0);

   public:
   static void GetOSEvents();
   static void RegisterForEvent(std::weak_ptr<EventListenerX11> listener, EventType eventType);
   static bool UnregisterForEvent(std::weak_ptr<EventListenerX11> listener, EventType eventType);
};

}  // namespace NLSWIN