#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Events/Event.hpp"
#include "NLSAPI.h"

namespace NLSWIN {

class Window;
struct NLSWIN_API_PUBLIC KeyboardDeviceInfo {
   std::string name;
   uint32_t platformSpecificIdentifier {0};
};

class NLSWIN_API_PUBLIC Keyboard {
   private:
   class Impl;
   std::shared_ptr<Impl> m_pImpl {nullptr};

   public:
   Keyboard(const Window &window);
   Keyboard(const Window &window, KeyboardDeviceInfo device);
   ~Keyboard();
   [[nodiscard]] bool HasEvent() const noexcept;
   Event GetNextEvent();

   [[nodiscard]] static std::vector<KeyboardDeviceInfo> EnumerateKeyboards() noexcept;
};

}  // namespace NLSWIN