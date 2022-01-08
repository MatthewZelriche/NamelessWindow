/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup X11 Linux X11 API
 * @brief Platform-specific X11 implementation of the API
 */
#pragma once

#include <memory>
#include <vector>

#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "X11EventListener.hpp"

namespace NLSWIN {

/*!
 * @brief Singleton which receives events from the X11 server and dispatches them to interested listeners.
 * @ingroup X11
 * @see EventBus
 * @see X11EventListener
 */
class NLSWIN_API_PRIVATE X11EventBus {
   public:
   /*! Singleton Accessor */
   static X11EventBus &GetInstance();
   /*!
    * @brief Dispatches all accumulated X events to interested listeners.
    *
    * This method ensures events are only sent to X11EventListeners that have explicitly subscribed to that
    * event by setting their xcb event mask.
    *
    * @post All events that were dispatched by the previous call to this method are freed.
    * @post Listeners who have since been deallocated are removed from the list of listeners.
    */
   void PollEvents();
   /*! Adds a new listener to the list of registered listeners */
   void RegisterListener(std::weak_ptr<X11EventListener> listener);
   /*! Removes a listener from the list of registered listeners */
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