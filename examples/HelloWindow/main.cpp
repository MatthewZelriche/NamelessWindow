#include <iostream>
#include <memory>
#include <vector>

#include "NamelessWindow/Window.hpp"

int main() {
   std::unique_ptr<NLSWIN::Window> window = NLSWIN::Window::CreateWindow();
   std::vector<NLSWIN::Screen> screens    = NLSWIN::Window::EnumerateScreens();
   for (auto screen: screens) {
      std::cout << screen.name << std::endl;
      std::cout << screen.verticalResolution << std::endl;
      std::cout << screen.horzResolution << std::endl;
   }
}