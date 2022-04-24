#pragma once

#include <memory>
#include "W32Window.hpp"

#include "NamelessWindow/NLSAPI.hpp"
#include "NamelessWindow/Events/Event.hpp"

namespace NLSWIN {
/*! @ingroup WIN32 */
class NLSWIN_API_PRIVATE W32BaseMouse {
   public:
   W32BaseMouse();

   protected:
   Event PackageRawDeltaEvent(RAWMOUSE mouse);

   private:
   static bool s_firstInit;
   static std::shared_ptr<W32Window> s_rawInputHandle;
};
}  // namespace NLSWIN