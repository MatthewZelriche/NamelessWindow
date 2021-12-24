#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Events/Event.hpp"
#include "NLSAPI.h"

namespace NLSWIN {

class Window;
struct NLSWIN_API_PUBLIC KeyboardDeviceInfo {
   const std::string_view name {""};
   const uint32_t platformSpecificIdentifier {0};
};

class NLSWIN_API_PUBLIC Keyboard {
   private:
   class Impl;
   std::shared_ptr<Impl> m_pImpl {nullptr};
   friend class Window;

   public:
   Keyboard();
   Keyboard(KeyboardDeviceInfo device);
   ~Keyboard();
   void SubscribeToWindow(const Window &window);
   [[nodiscard]] bool HasEvent() const noexcept;
   Event GetNextEvent();

   [[nodiscard]] static std::vector<KeyboardDeviceInfo> EnumerateKeyboards() noexcept;
};

}  // namespace NLSWIN