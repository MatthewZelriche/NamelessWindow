#include <iostream>
#include <memory>
#include <vector>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventQueue.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   NLSWIN::WindowProperties properties;
   properties.horzResolution = 800;
   properties.vertResolution = 600;
   properties.windowName = "Hello World!";
   properties.mode = NLSWIN::WindowMode::WINDOWED;

   NLSWIN::Window window(properties);
   // We don't care which keyboard we wish to get events from...
   NLSWIN::Keyboard keyboard;
   keyboard.SubscribeToWindow(window);

   while (!window.RequestedClose()) {
      NLSWIN::EventQueue::GetOSEvents();

      while (keyboard.HasEvent()) {
         NLSWIN::Event nextEvent = keyboard.GetNextEvent();

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