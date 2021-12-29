#include <iostream>

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

   std::shared_ptr<NLSWIN::Window> window = NLSWIN::Window::Create(properties);
   std::shared_ptr<NLSWIN::Keyboard> keyboard = NLSWIN::Keyboard::Create();
   keyboard->SubscribeToWindow(window.get());
   while (!window->RequestedClose()) {
      NLSWIN::EventQueue::GetOSEvents();

      while (window->HasEvent()) {
         NLSWIN::Event nextEvent = window->GetNextEvent();

         // Example event handling
         if (auto resizeEvent = std::get_if<NLSWIN::WindowResizeEvent>(&nextEvent)) {
            std::cout << "Window resized!" << std::endl;
         }
      }
   }
}