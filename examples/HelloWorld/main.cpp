#include <iostream>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   NLSWIN::EventBus::PollEvents();
   auto window = NLSWIN::Window::Create();
   window->Show();
   auto window2 = NLSWIN::Window::Create();
   window2->DisableUserResizing();
   window2->Show();
   auto keyboard = NLSWIN::Keyboard::Create();
   keyboard->SubscribeToWindow(window2.get());
   while (!window->RequestedClose()) {
      NLSWIN::EventBus::PollEvents();
      while (keyboard->HasEvent()) {
         auto bub = keyboard->GetNextEvent();

         if (auto keyEvent = std::get_if<NLSWIN::KeyEvent>(&bub)) {
            std::cout << keyEvent->keyName << std::endl;
         }
      }

      while (window->HasEvent()) {
         auto bub = window->GetNextEvent();
         if (auto resize = std::get_if<NLSWIN::WindowResizeEvent>(&bub)) {
            std::cout << "Resize!" << std::endl;
         }
      }
   }
}