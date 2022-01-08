#pragma once

#include <xcb/xcb.h>

#include <memory>
#include <queue>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventListener.hpp"
#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE X11EventListener : virtual public EventListener {
   public:
   [[nodiscard]] bool HasEvent() const noexcept override;
   [[nodiscard]] Event GetNextEvent() override;
   virtual void ProcessGenericEvent(xcb_generic_event_t *event) = 0;
   inline xcb_event_mask_t GetSubscribedEvents() const noexcept { return m_subscribedEventMask; }

   protected:
   void PushEvent(Event event);
   void SubscribeToEvents(xcb_event_mask_t mask);

   private:
   xcb_event_mask_t m_subscribedEventMask;
   std::queue<Event> m_Queue;
};

}  // namespace NLSWIN