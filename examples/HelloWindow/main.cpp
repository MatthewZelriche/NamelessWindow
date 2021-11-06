#include <iostream>
#include <memory>

#include "NamelessWindow/Window.hpp"

int main() {
   std::unique_ptr<NLSWIN::Window> window = NLSWIN::Window::CreateWindow();
   int result                             = window->SetSomething(5);
   std::cout << result << std::endl;
}