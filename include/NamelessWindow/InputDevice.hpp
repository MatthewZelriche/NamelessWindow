/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup Common Public API
 * @brief Documentation for public API that clients directly interact with.
 */
#pragma once

#include "Events/EventListener.hpp"
#include "Window.hpp"

namespace NLSWIN {

class NLSWIN_API_PUBLIC InputDevice : virtual public EventListener {
   public:
   /*!
    * @brief Subscribe this InputDevice to receive events from a specified window.
    *
    * InputDevices only receive events of windows that they are subscribed to. In order to receive input
    * events, you must first subscribe the InputDevice to at least one window.
    *
    * @param window The window to subscribe to for events.
    */
   virtual void SubscribeToWindow(const std::weak_ptr<Window> window) = 0;
   /*!
    * @brief Unsubscribe this InputDevice from receiving events from the specified window.
    *
    * @param window The window to unsubscribe from for events.
    */
   virtual void UnsubscribeFromWindow(const std::weak_ptr<Window> window) = 0;
};
}  // namespace NLSWIN