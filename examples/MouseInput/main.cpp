#include <iostream>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventQueue.hpp"
#include "NamelessWindow/Pointer.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   NLSWIN::WindowProperties properties;
   properties.horzResolution = 800;
   properties.vertResolution = 600;
   properties.windowName = "Hello World!";
   properties.mode = NLSWIN::WindowMode::WINDOWED;

   auto pointers = NLSWIN::Pointer::EnumeratePointers();

   for (auto pointer: pointers) { std::cout << pointer.name << std::endl; }

   NLSWIN::Window window(properties);
   NLSWIN::Pointer pointer;
   pointer.SubscribeToWindow(window);
   while (!window.RequestedClose()) {
      NLSWIN::EventQueue::GetOSEvents();

      while (pointer.HasEvent()) {
         NLSWIN::Event nextEvent = pointer.GetNextEvent();

         if (auto buttonEvent = std::get_if<NLSWIN::MouseButtonEvent>(&nextEvent)) {
            if (buttonEvent->type == NLSWIN::ButtonPressType::PRESSED) {
               std::cout << (int)buttonEvent->button << "Pressed at coordinates x: " << buttonEvent->xPos
                         << ", y: " << buttonEvent->yPos << std::endl;
            }
         }
      }
   }
}