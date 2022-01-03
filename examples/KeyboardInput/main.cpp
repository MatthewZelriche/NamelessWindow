#include <iostream>
#include <memory>
#include <vector>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventDispatcher.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   NLSWIN::WindowProperties properties;
   properties.horzResolution = 800;
   properties.vertResolution = 600;
   properties.windowName = "Hello World!";
   properties.mode = NLSWIN::WindowMode::WINDOWED;

   std::shared_ptr<NLSWIN::Window> window = NLSWIN::Window::Create(properties);
   // We don't care which keyboard we wish to get events from...
   std::shared_ptr<NLSWIN::Keyboard> keyboard = NLSWIN::Keyboard::Create();
   keyboard->SubscribeToWindow(window.get());

   while (!window->RequestedClose()) {
      NLSWIN::EventDispatcher::GetOSEvents();

      while (keyboard->HasEvent()) {
         NLSWIN::Event nextEvent = keyboard->GetNextEvent();

         // Example event handling
         if (auto keyEvent = std::get_if<NLSWIN::KeyEvent>(&nextEvent)) {
            if (keyEvent->pressType == NLSWIN::KeyPressType::PRESSED) {
               std::cout << keyEvent->keyName << " pressed!" << std::endl;
            } else if (keyEvent->pressType == NLSWIN::KeyPressType::REPEAT) {
               std::cout << keyEvent->keyName << " repeat!" << std::endl;
            } else if (keyEvent->pressType == NLSWIN::KeyPressType::RELEASED) {
               std::cout << keyEvent->keyName << " released!" << std::endl;
            }
         }
      }
   }
}