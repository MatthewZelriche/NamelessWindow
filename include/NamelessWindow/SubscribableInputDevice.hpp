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

/**
 * @brief Represents an input device that must subscribe to application windows to receive input
 * events from those windows.
 * @ingroup Common
 *
 * Subscribable Input Devices are contrasted with other devices such as Cursor and RawMouse. While
 * these devices receive events from all application windows automatically, Subscribable devices are more
 * selective. Before a Subscribable Input Device can receive input events, it must first register with at
 * least one application window. These Input Devices will only ever receive input events from windows they
 * have registered with. For example, an application may have three windows, but a Keyboard object that only
 * registered to one window. Keypresses that occur while the other two windows are focused will not be sent to
 * the Keyboard object.
 */
class NLSWIN_API_PUBLIC SubscribableInputDevice : virtual public EventListener {
   public:
   /*!
    * @brief Subscribe this Device to receive events from a specified application window.
    *
    * InputDevices only receive events of windows that they are subscribed to. In order to receive input
    * events, you must first subscribe the InputDevice to at least one window.
    *
    * @param window The application window to subscribe to for events.
    */
   virtual void SubscribeToWindow(const std::weak_ptr<Window> window) = 0;
   /*!
    * @brief Unsubscribe this Device from receiving events from the specified window.
    *
    * @param window The application window to unsubscribe from for events.
    */
   virtual void UnsubscribeFromWindow(const std::weak_ptr<Window> window) = 0;
};
}  // namespace NLSWIN