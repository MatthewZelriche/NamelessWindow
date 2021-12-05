#pragma once

#include <memory>
#include <string>
#include <vector>

namespace NLSWIN {

struct KeyboardDeviceInfo {
   std::string name;
   uint32_t platformSpecificIdentifier;
};

class Keyboard {
   private:
   class KeyboardImpl;
   std::shared_ptr<KeyboardImpl> m_pImpl {nullptr};

   public:
   Keyboard();
   ~Keyboard();
   static std::vector<KeyboardDeviceInfo> EnumerateKeyboards();
};

}  // namespace NLSWIN