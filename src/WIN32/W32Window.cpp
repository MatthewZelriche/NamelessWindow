#include "W32Window.hpp"

using namespace NLSWIN;

std::shared_ptr<NLSWIN::Window> NLSWIN_API_PRIVATE NLSWIN::Window::Create() {
   std::shared_ptr<W32Window> impl = std::make_shared<W32Window>(WindowProperties());
   // TODO: Register? Will we use the same format as Linux?
   return std::move(impl);
}

std::shared_ptr<NLSWIN::Window> NLSWIN::Window::Create(WindowProperties properties) {
   std::shared_ptr<W32Window> impl = std::make_shared<W32Window>(properties);
   // TODO: Register? Will we use the same format as Linux?
   return std::move(impl);
}

NLSWIN_API_PRIVATE W32Window::W32Window(WindowProperties properties) {
}

void W32Window::Show() {
}
void W32Window::Hide() {
}
void W32Window::DisableUserResizing() {
}
void W32Window::EnableUserResizing() {
}
void W32Window::SetFullscreen(bool borderless) noexcept {
}
void W32Window::SetWindowed() noexcept {
}
void W32Window::Reposition(uint32_t newX, uint32_t newY) noexcept {
}
void W32Window::Resize(uint32_t width, uint32_t height) noexcept {
}