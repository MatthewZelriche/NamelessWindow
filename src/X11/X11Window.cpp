#include "X11Window.hpp"

#include <memory>

#include "X11EventBus.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::shared_ptr<NLSWIN::Window> NLSWIN::Window::Create() {
   std::shared_ptr<X11Window> impl = std::make_shared<X11Window>(WindowProperties());
   X11EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

std::shared_ptr<NLSWIN::Window> NLSWIN::Window::Create(WindowProperties properties) {
   std::shared_ptr<X11Window> impl = std::make_shared<X11Window>(properties);
   X11EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

X11Window::X11Window(WindowProperties properties) {
   SubscribeToEvents(m_eventMask);
}

void X11Window::ProcessGenericEvent(xcb_generic_event_t *event) {
}