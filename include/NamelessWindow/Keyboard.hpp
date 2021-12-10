#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Event.hpp"

namespace NLSWIN {

class Window;

struct KeyboardDeviceInfo {
   std::string name;
   uint32_t platformSpecificIdentifier;
};

class Keyboard {
   private:
   class KeyboardImpl;
   std::shared_ptr<KeyboardImpl> m_pImpl {nullptr};

   public:
   Keyboard(const Window &window);
   Keyboard(const Window &window, KeyboardDeviceInfo device);
   ~Keyboard();
   static std::vector<KeyboardDeviceInfo> EnumerateKeyboards();
   bool HasEvent();
   Event GetNextEvent();
};

}  // namespace NLSWIN