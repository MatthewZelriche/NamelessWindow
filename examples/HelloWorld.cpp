#include <iostream>

#include "NamelessWindow/Cursor.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/RawMouse.hpp"
#include "NamelessWindow/Rendering/GLContext.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   auto infos = NLSWIN::Keyboard::EnumerateKeyboards();
   auto mouseInfos = NLSWIN::RawMouse::EnumeratePointers();
   auto kb = NLSWIN::Keyboard::Create();
   // auto kb2 = NLSWIN::Keyboard::Create(infos[1]);
   //  auto rawMouse = NLSWIN::RawMouse::Create(mouseInfos[0]);
   auto cursor = NLSWIN::Cursor::Create();

   NLSWIN::WindowProperties props;
   props.windowName = "Hello world!";
   // props.mode = NLSWIN::WindowMode::FULLSCREEN;
   auto window = NLSWIN::Window::Create(props);
   window->Show();
   auto win2 = NLSWIN::Window::Create();
   win2->Show();

   auto context = NLSWIN::GLContext::Create(window);

   kb->SubscribeToWindow(window);
   kb->SubscribeToWindow(win2);
   // kb2->SubscribeToWindow(window);

   // cursor->Hide();
   cursor->BindToWindow(win2.get());

   while (!window->RequestedClose()) {
      NLSWIN::EventBus::PollEvents();

      if (win2 && win2->RequestedClose()) {
         win2.reset();
      }

      while (window && window->HasEvent()) {
         auto event = window->GetNextEvent();
         if (auto eventt = std::get_if<NLSWIN::WindowFocusedEvent>(&event)) {
            std::cout << "grub" << std::endl;
         }
      }

      while (win2 && win2->HasEvent()) {
         auto event = win2->GetNextEvent();

         if (auto resizeEvent = std::get_if<NLSWIN::WindowResizeEvent>(&event)) {
            // std::cout << resizeEvent->newWidth << ", " << resizeEvent->newHeight
            //           << ", Window: " << resizeEvent->sourceWindow << std::endl;
         } else if (auto focusEvent = std::get_if<NLSWIN::WindowFocusedEvent>(&event)) {
            // std::cout << "Focused on: " << focusEvent->sourceWindow << std::endl;
         }
      }
      while (kb->HasEvent()) {
         auto event = kb->GetNextEvent();
         if (auto keyEvent = std::get_if<NLSWIN::KeyEvent>(&event)) {
            if (keyEvent->pressType == NLSWIN::KeyPressType::PRESSED) {
               // window->SetFullscreen(false);
               // window->Resize(1920, 1080);
               window->Focus();
            } else if (keyEvent->pressType == NLSWIN::KeyPressType::RELEASED) {
               // window->SetWindowed();
               // window->Resize(680, 480);
            }
            std::cout << "Hello from keyboard one: " << keyEvent->keyName << std::endl;
            std::cout << keyEvent->sourceWindow << std::endl;
         }
      }
   }
}