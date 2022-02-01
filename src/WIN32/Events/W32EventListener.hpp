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
#include "NamelessWindow/Events/EventListener.hpp"
#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE W32EventListener : public virtual EventListener {
   public:
   [[nodiscard]] bool HasEvent() const noexcept override;
   [[nodiscard]] Event GetNextEvent() override;
};
}  // namespace NLSWIN