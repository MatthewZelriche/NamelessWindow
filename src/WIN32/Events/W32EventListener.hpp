/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup WIN32 Windows API
 * @brief Platform-specific Windows implementation of the API
 */

#pragma once
#include <windows.h>

#include <queue>

#include "NamelessWindow/Events/EventListener.hpp"
#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE W32EventListener : public virtual EventListener {
   public:
   [[nodiscard]] bool HasEvent() const noexcept override;
   [[nodiscard]] Event GetNextEvent() override;

   /*!
    * @brief Takes a Win32 event received from the EventBus, and either constructs a platform-independent
    * Event object to store in its queue, or discards the event.
    *
    * @param event The generic event received from the EventBus to process.
    */
   virtual void ProcessGenericEvent(MSG event) = 0;

   protected:
   /*!
    * @brief Push a new processed platform-independent event onto this listener's queue of events.
    *
    * @param event The event to push.
    */
   void PushEvent(Event event);

   private:
   std::queue<Event> m_Queue;
};
}  // namespace NLSWIN