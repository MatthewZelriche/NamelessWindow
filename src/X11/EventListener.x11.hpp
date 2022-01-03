#pragma once

#include <xcb/xcb.h>

#include <queue>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventListener.hpp"
#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE EventListenerX11 : virtual public EventListener {
   private:
   std::queue<Event> m_Queue;

   public:
   [[nodiscard]] bool HasEvent() const noexcept override;
   [[nodiscard]] Event GetNextEvent() override;
   void PushEvent(Event event);
   virtual void ProcessGenericEvent(xcb_generic_event_t *event) = 0;
};

}  // namespace NLSWIN