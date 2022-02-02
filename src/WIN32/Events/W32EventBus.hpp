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
#include <memory>
#include <vector>

#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "W32EventListener.hpp"

namespace NLSWIN {
/*!
 * @brief Dispatches Win32 queued events.
 * @ingroup WIN32
 */
class NLSWIN_API_PRIVATE W32EventBus {
   public:
   /*! Singleton Accessor */
   static W32EventBus &GetInstance();
   void PollEvents();

   /*! Adds a new listener to the list of registered listeners */
   void RegisterListener(std::weak_ptr<W32EventListener> listener);

   private:
   std::vector<std::weak_ptr<W32EventListener>> m_listeners;
};

}  // namespace NLSWIN