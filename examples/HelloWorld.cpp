#include <iostream>

#include "NamelessWindow/Cursor.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/RawMouse.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   auto infos = NLSWIN::Keyboard::EnumerateKeyboards();
   auto kb = NLSWIN::Keyboard::Create(infos[0]);

   NLSWIN::WindowProperties props;
   props.windowName = "Hello world!";
   auto window = NLSWIN::Window::Create(props);
   window->Show();

   kb->SubscribeToWindow(window);
   //kb2->SubscribeToWindow(window);
   while (!window->RequestedClose()) {
      NLSWIN::EventBus::PollEvents();

      while (window->HasEvent()) {
         auto event = window->GetNextEvent();

         if (auto resizeEvent = std::get_if<NLSWIN::WindowResizeEvent>(&event)) {
            std::cout << resizeEvent->newWidth << ", " << resizeEvent->newHeight
                      << ", Window: " << resizeEvent->sourceWindow << std::endl;
         }
      }
      while (kb->HasEvent()) {
          auto event = kb->GetNextEvent();
         if (auto keyEvent = std::get_if<NLSWIN::KeyEvent>(&event)) {
            std::cout << "Hello from keyboard one: " << keyEvent->keyName << std::endl;
         }
      }
      /*
      while (kb2->HasEvent()) {
          auto event = kb2->GetNextEvent();
         if (auto keyEvent = std::get_if<NLSWIN::KeyEvent>(&event)) {
            std::cout << "Hello from keyboard two: " << keyEvent->keyName << std::endl;
         }
      }
      */
   }
}