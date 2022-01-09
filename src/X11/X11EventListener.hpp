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

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <memory>
#include <queue>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventListener.hpp"
#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN {
/*!
 * @brief An interface implemented by all classes who wish to receive X events.
 * @ingroup X11
 * @see X11EventBus
 */
class NLSWIN_API_PRIVATE X11EventListener : virtual public EventListener {
   public:
   /*!
    * @brief Check if the listener has an event in its queue.
    *
    * @returns True if there exists an event in the queue, false otherwise.
    */
   [[nodiscard]] bool HasEvent() const noexcept override;
   /*!
    * @brief Gets the next event in the queue.
    * @throws EmptyEventQueueException
    *
    * @return The next event.
    */
   [[nodiscard]] Event GetNextEvent() override;
   /*!
    * @brief Takes a generic X event, and either constructs a platform-independent Event object to store in
    * its queue, or discards the event.
    *
    * @param event The generic X event received from the X11EventBus to process.
    */
   virtual void ProcessGenericEvent(xcb_generic_event_t *event) = 0;
   /*!
    * @brief Get a mask of xcb event types that this listener is currently subscribed to receive.
    *
    * @return The  xcb_input_xi_event_mask_t containing the XInput2 event types.
    */
   inline xcb_input_xi_event_mask_t GetSubscribedXInputEventTypes() const noexcept {
      return m_subscribedXInputMask;
   }
   /*!
    * @brief Get a mask of XInput2 event types that this listener is currently subscribed to receive.
    *
    * @return The xcb_event_mast_t containing the event types.
    */
   inline xcb_event_mask_t GetSubscribedEventTypes() const noexcept { return m_subscribedEventMask; }

   protected:
   /*!
    * @brief Push a new processed platform-independent event onto this listener's queue of events.
    *
    * @param event The event to push.
    */
   void PushEvent(Event event);
   /*!
    * @brief Subscribes this listener to each generic XCB event type specified in mask.
    *
    * @param mask The mask of event types to subscribe to.
    */
   virtual void SubscribeToEvents(xcb_event_mask_t mask);
   /*!
    * @brief Subscribes this listener to each XInput2 event type specified in mask.
    *
    * @param mask The mask of event types to subscribe to.
    */
   virtual void SubscribeToEvents(xcb_input_xi_event_mask_t mask);

   private:
   xcb_event_mask_t m_subscribedEventMask {XCB_EVENT_MASK_NO_EVENT};
   xcb_input_xi_event_mask_t m_subscribedXInputMask {(xcb_input_xi_event_mask_t)0};
   std::queue<Event> m_Queue;
};

}  // namespace NLSWIN