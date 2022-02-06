#pragma once

#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "W32InputDevice.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE W32Keyboard : virtual public Keyboard, public W32InputDevice {
   public:
   static std::shared_ptr<Keyboard> Create();
   static std::shared_ptr<Keyboard> Create(KeyboardDeviceInfo device);
};

}  // namespace NLSWIN