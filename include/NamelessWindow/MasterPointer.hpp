#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Events/Event.hpp"
#include "NLSAPI.h"
#include "RawPointer.hpp"

namespace NLSWIN {
class Window;
class NLSWIN_API_PUBLIC MasterPointer {
   public:
   ~MasterPointer();
   static MasterPointer &GetInstance();
   [[nodiscard]] bool HasEvent() const noexcept;
   Event GetNextEvent();
   void Enable();
   void Disable();

   private:
   class Impl;
   friend class Window;
   std::shared_ptr<Impl> m_pImpl {nullptr};

   MasterPointer();
};

}  // namespace NLSWIN