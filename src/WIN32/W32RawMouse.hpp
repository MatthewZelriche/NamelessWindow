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

#include "Events/W32EventListener.hpp"
#include "NamelessWindow/RawMouse.hpp"
#include "W32Window.hpp"

namespace NLSWIN {

/*! @ingroup WIN32 */
class NLSWIN_API_PRIVATE W32RawMouse : public RawMouse, public W32EventListener {
   public:
   W32RawMouse(MouseDeviceInfo device);

   private:
   void ProcessGenericEvent(MSG event) override;

   private:
   uint64_t m_deviceSpecifier {0};

   static bool s_firstInit;
   static std::shared_ptr<W32Window> s_rawInputHandle;
};

}  // namespace NLSWIN