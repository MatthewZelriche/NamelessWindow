#include "X11GLContext.hpp"

#include "../X11EventBus.hpp"
#include "../X11Util.hpp"
#include "../XConnection.h"
#include "NamelessWindow/Exceptions.hpp"

using namespace NLSWIN;

std::unique_ptr<GLContext> GLContext::Create(const std::shared_ptr<const Window> window) {
   // Cast to child class weak_ptr
   // We need a weak_ptr to test to see if the window has been destroyed by the user or not during calls like
   // MakeContextCurrent - to avoid glx errors.
   return std::make_unique<X11GLContext>(std::static_pointer_cast<const X11Window>(window));
}

X11GLContext::X11GLContext(std::weak_ptr<const X11Window> window) {
   m_xcbWindow = window;
   auto windowSharedPtr = m_xcbWindow.lock();
   // Get GLXFBConfig matching the one used by this window
   int numItems = 0;
   GLXFBConfig* configs = glXChooseFBConfig(XConnection::GetDisplay(), UTIL::GetDefaultScreenNumber(),
                                            windowSharedPtr->GetVisualAttributes().data(), &numItems);
   // Find the returned FBConfig that matches the visual used by this window.
   for (int i = 0; i < numItems; i++) {
      int id = 0;
      glXGetFBConfigAttrib(XConnection::GetDisplay(), configs[i], GLX_VISUAL_ID, &id);
      auto bab = windowSharedPtr->GetSelectedVisualID();
      if (id == windowSharedPtr->GetSelectedVisualID()) {
         m_chosenConfig = configs[i];
         break;
      }
   }
   if (!m_chosenConfig) {
      throw RenderContextInitFailureException();
   }

   m_context = glXCreateNewContext(XConnection::GetDisplay(), configs[0], GLX_RGBA_TYPE, nullptr, true);
   if (!m_context) {
      throw RenderContextInitFailureException();
   }

   m_xcbWindowID = windowSharedPtr->GetX11ID();
   m_glxWindow = glXCreateWindow(XConnection::GetDisplay(), m_chosenConfig, (XID)m_xcbWindowID, nullptr);
   if (!m_glxWindow) {
      glXDestroyContext(XConnection::GetDisplay(), m_context);
      throw RenderContextInitFailureException();
   }
}

void X11GLContext::MakeContextCurrent() {
   if (m_xcbWindow.expired()) {
      throw InvalidRenderContextStateException();
   }
   glXMakeContextCurrent(XConnection::GetDisplay(), m_glxWindow, m_glxWindow, m_context);
}

void X11GLContext::SwapContextBuffers() {
   if (m_xcbWindow.expired()) {
      throw InvalidRenderContextStateException();
   }
   glXSwapBuffers(XConnection::GetDisplay(), m_glxWindow);
}

void X11GLContext::SetVSync(bool state) {
   // LINUX UPDATE TODO
}