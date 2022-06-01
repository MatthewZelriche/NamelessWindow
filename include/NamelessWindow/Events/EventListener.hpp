/*!
 * @file EventListener.hpp
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup Common Public API
 * @brief Documentation for public API that clients directly interact with.
 */
#pragma once

#include "../NLSAPI.hpp"
#include "Event.hpp"

namespace NLSWIN {

/*!
 * @interface EventListener "Events/EventListener.hpp"
 * @ingroup Common
 * @brief Enables a class to receive events from the EventDispatcher
 *
 * All classes capable of processing OS events implement this interface. Each EventListener maintains its own
 * internal queue of events that the listener has processed. Registration of listeners implemented
 * by this library (eg, Window, Keyboard, etc) occurs automatically on instantiation of the listener. It is
 * not supported for the client to register their own custom EventListeners.
 * @see EventDispatcher
 */
class NLSWIN_API_PUBLIC EventListener {
   public:
   /*!
    * @brief Whether this listener holds at least one pending event.
    * @returns True if the listener has at least one pending event, False otherwise.
    */
   [[nodiscard]] virtual bool HasEvent() const noexcept = 0;
   /*!
    * @brief Pop (remove) an event from the listener's queue of pending events.
    * @post The size of the internal queue is decreased by one.
    * @throws EmptyEventQueueException
    * @returns The next pending event.
    */
   [[nodiscard]] virtual Event GetNextEvent() = 0;

   virtual ~EventListener() = default;
};

}  // namespace NLSWIN