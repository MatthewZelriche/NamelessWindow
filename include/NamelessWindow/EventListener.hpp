#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Events/Event.hpp"
#include "NLSAPI.h"

namespace NLSWIN {

class NLSWIN_API_PUBLIC EventListener {
   public:
   [[nodiscard]] virtual bool HasEvent() const noexcept = 0;
   [[nodiscard]] virtual Event GetNextEvent() = 0;
   virtual void PushEvent(Event event) = 0;
};

}  // namespace NLSWIN