#include <iostream>

#include "NamelessWindow/Cursor.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/RawMouse.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   NLSWIN::WindowProperties props;
   props.windowName = "Hello world!";
   auto window = NLSWIN::Window::Create(props);
   window->Show();

   while (window->RequestedClose()) {
      NLSWIN::EventBus::PollEvents();

      while (window->HasEvent()) { auto event = window->GetNextEvent(); }
   }
}