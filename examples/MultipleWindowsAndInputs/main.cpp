#include <iostream>
#include <memory>
#include <vector>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventDispatcher.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

/**
 * NOTE: This example simply shows how to use the API with multiple input devices. It does not include any
 * method of selecting an appropriate keyboard, or confirming multiple keyboards exist at all, and is
 * therefore unlikely to execute properly on other computers. In a real-world use case, you would likely need
 * some way of letting the application user select an appropriate keyboard returned from EnumerateKeyboards.
 */
std::shared_ptr<NLSWIN::Keyboard> ConstructKeyboardIfExists(const char *keyboardName) {
   auto keyboardInfos = NLSWIN::Keyboard::EnumerateKeyboards();
   for (auto keyboard: keyboardInfos) {
      // Specific to developer's computer - real-world use case would require some kind of interactive
      // selection here.
      if (keyboard.name == keyboardName) {
         return NLSWIN::Keyboard::Create(keyboard);
      }
   }
   return NLSWIN::Keyboard::Create();
}

int main() {
   NLSWIN::WindowProperties properties;
   properties.horzResolution = 800;
   properties.vertResolution = 600;
   properties.windowName = "Hello World!";
   properties.mode = NLSWIN::WindowMode::WINDOWED;

   std::shared_ptr<NLSWIN::Window> window = NLSWIN::Window::Create(properties);
   std::shared_ptr<NLSWIN::Window> window2 = NLSWIN::Window::Create();

   // We will set each keyboard to listen to a seperate window.
   auto keyboard = ConstructKeyboardIfExists("AT Translated Set 2 keyboard");
   keyboard->SubscribeToWindow(window.get());
   auto keyboard2 = ConstructKeyboardIfExists("MOSART Semi. 2.4G Keyboard Mouse");
   keyboard2->SubscribeToWindow(window2.get());

   while (!window->RequestedClose() && !window2->RequestedClose()) {
      NLSWIN::EventDispatcher::GetOSEvents();

      while (keyboard2->HasEvent()) {
         NLSWIN::Event nextEvent = keyboard2->GetNextEvent();

         if (auto keyEvent = std::get_if<NLSWIN::KeyEvent>(&nextEvent)) {
            if (keyEvent->sourceWindow == window2->GetWindowID()) {
               std::cout << "Window 2 recieved keypress!" << std::endl;
            }
         }
      }

      while (keyboard->HasEvent()) {
         NLSWIN::Event nextEvent = keyboard->GetNextEvent();

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