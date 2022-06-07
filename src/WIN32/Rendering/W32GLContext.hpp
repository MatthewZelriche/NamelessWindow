#pragma once

#include "../W32Window.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "NamelessWindow/Rendering/GLContext.hpp"

namespace NLSWIN {

typedef void (*wglSwapIntervalEXT_PFN)(int);

/*! @ingroup WIN32 */
class NLSWIN_API_PRIVATE W32GLContext : public GLContext {
   public:
   W32GLContext(std::weak_ptr<const W32Window> window);
   ~W32GLContext();

   void MakeContextCurrent() override;
   void SwapContextBuffers() override;
   void SetVSync(bool state) override;

   private:
   HDC m_deviceContext {0};
   HGLRC m_glContext {0};
   wglSwapIntervalEXT_PFN swapFunc {nullptr};
};
}  // namespace NLSWIN