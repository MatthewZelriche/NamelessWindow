#include <iostream>

#include "NamelessWindow/Cursor.hpp"
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
   keyboard->SubscribeToWindow(window2);
   auto mouse = NLSWIN::Cursor::Create();
   mouse->SubscribeToWindow(window);
   mouse->SubscribeToWindow(window2);
   mouse->BindToWindow(window.get());
   mouse->HideCursor();
   while (!window->RequestedClose()) {
      NLSWIN::EventBus::PollEvents();

      if (window2 && window2->RequestedClose()) {
         window2.reset();
      }

      while (mouse->HasEvent()) {
         auto bub = mouse->GetNextEvent();

         if (auto buttonEvent = std::get_if<NLSWIN::MouseButtonEvent>(&bub)) {
            if (buttonEvent->type == NLSWIN::ButtonPressType::PRESSED) {
               std::cout << "Press Button " << (int)buttonEvent->button << std::endl;
            } else {
               std::cout << "Release Button " << (int)buttonEvent->button << std::endl;
            }
         } else if (auto scrollEvent = std::get_if<NLSWIN::MouseScrollEvent>(&bub)) {
            std::cout << "Scrolled!" << (int)scrollEvent->scrollType << std::endl;
         } else if (auto rawDelta = std::get_if<NLSWIN::RawMouseDeltaMovementEvent>(&bub)) {
            std::cout << "Raw: " << rawDelta->deltaX << ", " << rawDelta->deltaY << std::endl;
         } else if (auto accelDelta = std::get_if<NLSWIN::MouseDeltaMovementEvent>(&bub)) {
            std::cout << "Accel: " << accelDelta->deltaX << ", " << accelDelta->deltaY << std::endl;
         } else if (auto newPos = std::get_if<NLSWIN::MouseMovementEvent>(&bub)) {
            std::cout << "new pos: " << newPos->newXPos << ", " << newPos->newYPos << std::endl;
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