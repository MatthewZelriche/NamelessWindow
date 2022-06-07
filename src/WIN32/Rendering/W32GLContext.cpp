#include "W32GLContext.hpp"

#include "NamelessWindow/Exceptions.hpp"

using namespace NLSWIN;

std::unique_ptr<GLContext> GLContext::Create(const std::shared_ptr<const Window> window) {
   return std::make_unique<W32GLContext>(std::static_pointer_cast<const W32Window>(window));
}

W32GLContext::W32GLContext(std::weak_ptr<const W32Window> window) {
   auto windowPtr = window.lock();
   if (!windowPtr) {
      throw RenderContextInitFailureException();
   }

   // WARNING: TODO: Have to handle devContext safely across threads.
   m_deviceContext = windowPtr->GetDeviceContext();
   if (!m_deviceContext) {
      throw RenderContextInitFailureException();
   }

   m_glContext = wglCreateContext(m_deviceContext);
   if (!m_glContext) {
      throw RenderContextInitFailureException();
   }
}

void W32GLContext::SetVSync(bool state) {
   if (!swapFunc) {
      swapFunc = (wglSwapIntervalEXT_PFN)(wglGetProcAddress("wglSwapIntervalEXT"));
   }
   swapFunc(state);
}

W32GLContext::~W32GLContext() {
   if (wglGetCurrentContext() == m_glContext) {
      wglMakeCurrent(nullptr, nullptr);
   }
   wglDeleteContext(m_glContext);
}

void W32GLContext::MakeContextCurrent() {
   wglMakeCurrent(m_deviceContext, m_glContext);
}

void W32GLContext::SwapContextBuffers() {
   SwapBuffers(m_deviceContext);
}