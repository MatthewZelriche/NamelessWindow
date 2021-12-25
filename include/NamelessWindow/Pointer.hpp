#pragma once

#include <memory>
#include <vector>

#include "Events/Event.hpp"
#include "NLSAPI.h"

namespace NLSWIN {

class Window;
struct NLSWIN_API_PUBLIC PointerDeviceInfo {
   const std::string_view name {""};
   const uint32_t platformSpecificIdentifier {0};
};

class NLSWIN_API_PUBLIC Pointer {
   private:
   class Impl;
   std::shared_ptr<Impl> m_pImpl {nullptr};
   friend class Window;

   public:
   Pointer();
   Pointer(PointerDeviceInfo device);
   ~Pointer();
   void SubscribeToWindow(const Window &window);
   [[nodiscard]] bool HasEvent() const noexcept;
   Event GetNextEvent();

   [[nodiscard]] static std::vector<PointerDeviceInfo> EnumeratePointers() noexcept;
};

}  // namespace NLSWIN