#include <iostream>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   NLSWIN::EventBus::PollEvents();
   auto window = NLSWIN::Window::Create();
   window->Show();
   while (true) {
      NLSWIN::EventBus::PollEvents();
      while (window->HasEvent()) { auto bub = window->GetNextEvent(); }
   }
}