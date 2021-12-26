#include <iostream>
#include <memory>
#include <vector>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventQueue.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

/**
 * NOTE: This example simply shows how to use the API with multiple input devices. It does not include any
 * method of selecting an appropriate keyboard, or confirming multiple keyboards exist at all, and is
 * therefore unlikely to execute properly on other computers. In a real-world use case, you would likely need
 * some way of letting the application user select an appropriate keyboard returned from EnumerateKeyboards.
 */
NLSWIN::Keyboard ConstructKeyboardIfExists(const char *keyboardName) {
   auto keyboardInfos = NLSWIN::Keyboard::EnumerateKeyboards();
   for (auto keyboard: keyboardInfos) {
      // Specific to developer's computer - real-world use case would require some kind of interactive
      // selection here.
      if (keyboard.name == keyboardName) {
         return NLSWIN::Keyboard(keyboard);
      }
   }
   return NLSWIN::Keyboard();
}

int main() {
   NLSWIN::WindowProperties properties;
   properties.horzResolution = 800;
   properties.vertResolution = 600;
   properties.windowName = "Hello World!";
   properties.mode = NLSWIN::WindowMode::WINDOWED;

   NLSWIN::Window window(properties);
   NLSWIN::Window window2;

   // We will set each keyboard to listen to a seperate window.
   NLSWIN::Keyboard keyboard = ConstructKeyboardIfExists("AT Translated Set 2 keyboard");
   keyboard.SubscribeToWindow(window);
   NLSWIN::Keyboard keyboard2 = ConstructKeyboardIfExists("MOSART Semi. 2.4G Keyboard Mouse");
   keyboard2.SubscribeToWindow(window2);

   while (!window.RequestedClose() && !window2.RequestedClose()) {
      NLSWIN::EventQueue::GetOSEvents();

      while (keyboard2.HasEvent()) {
         NLSWIN::Event nextEvent = keyboard2.GetNextEvent();

         if (auto keyEvent = std::get_if<NLSWIN::KeyEvent>(&nextEvent)) {
            if (keyEvent->sourceWindow == window2.GetWindowID()) {
               std::cout << "Window 2 recieved keypress!" << std::endl;
            }
         }
      }

      while (keyboard.HasEvent()) {
         NLSWIN::Event nextEvent = keyboard.GetNextEvent();

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