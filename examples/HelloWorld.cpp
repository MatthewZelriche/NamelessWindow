#include <iostream>

#include "NamelessWindow/Cursor.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/RawMouse.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   auto infos = NLSWIN::Keyboard::EnumerateKeyboards();
   auto mouseInfos = NLSWIN::RawMouse::EnumeratePointers();
   auto kb = NLSWIN::Keyboard::Create(infos[0]);
   auto kb2 = NLSWIN::Keyboard::Create(infos[1]);
   auto rawMouse = NLSWIN::RawMouse::Create(mouseInfos[0]);

   NLSWIN::WindowProperties props;
   props.windowName = "Hello world!";
   //props.mode = NLSWIN::WindowMode::FULLSCREEN;
   auto window = NLSWIN::Window::Create(props);
   window->Show();
  // auto win2 = NLSWIN::Window::Create();
   //win2->Show();

   kb->SubscribeToWindow(window);
   kb2->SubscribeToWindow(window);
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
             if (keyEvent->pressType == NLSWIN::KeyPressType::PRESSED) {
               //window->SetFullscreen(false);
               //window->Resize(1920, 1080);
             } else if (keyEvent->pressType == NLSWIN::KeyPressType::RELEASED) {
                //window->SetWindowed();
                //window->Resize(680, 480);
             }
            std::cout << "Hello from keyboard one: " << keyEvent->keyName << std::endl;
         }
      }

      
      while (rawMouse->HasEvent()) { 
          auto event = rawMouse->GetNextEvent();
         if (auto rawMoveEvent = std::get_if<NLSWIN::RawMouseScrollEvent>(&event)) {
             std::cout << "bub" << std::endl;
         }
      }
      
      while (kb2->HasEvent()) {
          auto event = kb2->GetNextEvent();
         if (auto keyEvent = std::get_if<NLSWIN::KeyEvent>(&event)) {
            std::cout << "Hello from keyboard two: " << keyEvent->keyName << std::endl;
         }
      }
      
   }
}