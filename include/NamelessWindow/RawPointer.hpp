#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Events/Event.hpp"
#include "NLSAPI.h"

namespace NLSWIN {

class Window;
struct NLSWIN_API_PUBLIC PointerDeviceInfo {
   const std::string_view name {""};
   const uint32_t platformSpecificIdentifier {0};
};

class NLSWIN_API_PUBLIC RawPointer {
   private:
   class Impl;
   std::shared_ptr<Impl> m_pImpl {nullptr};

   public:
   RawPointer(PointerDeviceInfo device, const Window &window);
   ~RawPointer();
   void BindToWindow(const Window &window);
   [[nodiscard]] bool HasEvent() const noexcept;
   Event GetNextEvent();

   [[nodiscard]] static std::vector<PointerDeviceInfo> EnumeratePointers() noexcept;
};

}  // namespace NLSWIN