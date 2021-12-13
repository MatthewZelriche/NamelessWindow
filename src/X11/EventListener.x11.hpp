#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <queue>
#include <unordered_set>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/NLSAPI.h"

namespace NLSWIN {

class NLSWIN_API_PRIVATE EventListenerX11 {
   protected:
   std::queue<Event> m_Queue;

   public:
   [[nodiscard]] bool HasEvent() const noexcept;
   Event GetNextEvent();
   virtual void ProcessGenericEvent(xcb_generic_event_t *event) = 0;
};

}  // namespace NLSWIN