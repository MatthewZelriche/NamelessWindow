#include <iostream>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/RawMouse.hpp"
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
   auto mouse = NLSWIN::RawMouse::Create({"bab", 8});
   while (!window->RequestedClose()) {
      NLSWIN::EventBus::PollEvents();

      while (mouse->HasEvent()) {
         auto bub = mouse->GetNextEvent();

         if (auto buttonEvent = std::get_if<NLSWIN::RawMouseButtonEvent>(&bub)) {
            if (buttonEvent->type == NLSWIN::ButtonPressType::PRESSED) {
               std::cout << "Press Button " << (int)buttonEvent->button << std::endl;
            } else {
               std::cout << "Release Button " << (int)buttonEvent->button << std::endl;
            }
         } else if (auto scrollEvent = std::get_if<NLSWIN::RawMouseScrollEvent>(&bub)) {
            std::cout << "Scrolled!" << (int)scrollEvent->scrollType << std::endl;
         } else if (auto rawDelta = std::get_if<NLSWIN::RawMouseDeltaMovementEvent>(&bub)) {
            std::cout << "Raw: " << rawDelta->deltaX << ", " << rawDelta->deltaY << std::endl;
         } else if (auto accelDelta = std::get_if<NLSWIN::MouseDeltaMovementEvent>(&bub)) {
            std::cout << "Accel: " << accelDelta->deltaX << ", " << accelDelta->deltaY << std::endl;
         }
      }

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