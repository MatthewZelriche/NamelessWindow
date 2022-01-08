#pragma once

#include <memory>
#include <vector>

#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "X11EventListener.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE X11EventBus {
   public:
   static X11EventBus &GetInstance();
   void PollEvents();
   void RegisterListener(std::weak_ptr<X11EventListener> listener);
   void UnregisterListener(std::weak_ptr<X11EventListener> listener);

   private:
   std::vector<std::weak_ptr<X11EventListener>> m_listeners;
   std::vector<xcb_generic_event_t *> m_eventsToFreeNextPoll;
   void FreeOldEvents();
   X11EventBus() = default;
   X11EventBus(X11EventBus const &) = delete;
   void operator=(X11EventBus const &) = delete;
};

}  // namespace NLSWIN