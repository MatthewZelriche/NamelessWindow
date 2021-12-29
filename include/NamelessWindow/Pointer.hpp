#pragma once

#include <memory>
#include <string>
#include <vector>

#include "EventListener.hpp"
#include "Events/Event.hpp"
#include "NLSAPI.h"

namespace NLSWIN {

class Window;
struct NLSWIN_API_PUBLIC PointerDeviceInfo {
   const std::string_view name {""};
   const uint32_t platformSpecificIdentifier {0};
};

class NLSWIN_API_PUBLIC Pointer : virtual public EventListener {
   public:
   static std::shared_ptr<Pointer> Create(PointerDeviceInfo device, const Window *const window);
   ~Pointer();
   virtual void BindToWindow(const Window *const window) = 0;
   [[nodiscard]] static std::vector<PointerDeviceInfo> EnumeratePointers() noexcept;
};

}  // namespace NLSWIN