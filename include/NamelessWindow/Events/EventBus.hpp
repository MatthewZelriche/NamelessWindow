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
#include "../NLSAPI.hpp"

namespace NLSWIN {

/*!
 * @headerfile "Events/EventBus.hpp"
 * @ingroup Common
 * @brief Dispatches platform-specific events to interested listeners.
 *
 * The EventBus is a static class that retrieves the raw, untranslated OS events from the system and
 * dispatches then to all registered listeners so that the listeners may translate these OS Events into
 * platform-independent NLSWIN events.
 *
 * @see EventListener
 */
class NLSWIN_API_PUBLIC EventBus {
   public:
   /*!
    * @brief Non-Blocking retrieval of all OS events that have accumulated since the last call to this method.
    * @throws PlatformInitializationException
    *
    * Polls and dispatches all events that have been created by the OS, but not yet dispatched by this library
    * via a previous call to this method. This method is non-blocking. A common use case of this method is to
    * call it once per frame in a realtime 3D application.
    */
   static void PollEvents();
};

}  // namespace NLSWIN