#pragma once

#include <memory>
#include <string>
#include <vector>

#include "EventListener.hpp"
#include "Events/Event.hpp"
#include "NLSAPI.h"

namespace NLSWIN {

class Window;
struct NLSWIN_API_PUBLIC KeyboardDeviceInfo {
   const std::string_view name {""};
   const uint32_t platformSpecificIdentifier {0};
};

class NLSWIN_API_PUBLIC Keyboard : virtual public EventListener {
   public:
   static std::shared_ptr<Keyboard> Create();
   static std::shared_ptr<Keyboard> Create(KeyboardDeviceInfo device);
   ~Keyboard();
   virtual void SubscribeToWindow(const Window *const window) = 0;
   [[nodiscard]] static std::vector<KeyboardDeviceInfo> EnumerateKeyboards() noexcept;
};

}  // namespace NLSWIN