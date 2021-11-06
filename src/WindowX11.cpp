#include "WindowX11.hpp"

#include <memory>

using namespace NLSWIN;

std::unique_ptr<Window> Window::CreateWindow() {
   return std::make_unique<WindowX11>();
}

WindowX11::WindowX11() {
}

int WindowX11::SetSomething(int value) {
   return value * 3;
}