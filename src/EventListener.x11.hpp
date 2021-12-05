#pragma once

#include <xcb/xcb.h>

#include <queue>
#include <unordered_set>

#include "NamelessWindow/Event.hpp"

namespace NLSWIN {

class EventListenerX11 {
   private:
   std::queue<Event> m_Queue;

   protected:
   std::unordered_set<xcb_window_t> m_windows;

   public:
   virtual void EventRecieved(Event event);
   bool HasEvent();
   Event GetNextEvent();

   inline std::unordered_set<xcb_window_t> &GetWindows() { return m_windows; }
};

}  // namespace NLSWIN