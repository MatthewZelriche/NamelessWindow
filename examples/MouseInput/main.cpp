#include <iostream>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventQueue.hpp"
#include "NamelessWindow/RawPointer.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   NLSWIN::Window window;

   // List enabled pointer devices.
   auto pointerInfos = NLSWIN::RawPointer::EnumeratePointers();
   for (auto info: pointerInfos) { std::cout << info.name << std::endl; }

   while (!window.RequestedClose()) {
      NLSWIN::EventQueue::GetOSEvents();

      while (window.HasEvent()) {
         NLSWIN::Event nextEvent = window.GetNextEvent();

         if (auto rawDelta = std::get_if<NLSWIN::MouseRawDeltaMovementEvent>(&nextEvent)) {
            std::cout << "Raw Delta: " << rawDelta->deltaX << ", " << rawDelta->deltaY << std::endl;
         } else if (auto delta = std::get_if<NLSWIN::MouseDeltaMovementEvent>(&nextEvent)) {
            std::cout << "Accel Delta: " << delta->deltaX << ", " << delta->deltaY << std::endl;
         } else if (auto pos = std::get_if<NLSWIN::MouseMovementEvent>(&nextEvent)) {
            std::cout << "Mouse current pos: " << pos->newXPos << ", " << pos->newYPos << std::endl;
         } else if (auto mouseClick = std::get_if<NLSWIN::MouseButtonEvent>(&nextEvent)) {
            if (mouseClick->type == NLSWIN::ButtonPressType::PRESSED) {
               std::cout << (int)mouseClick->button << " clicked!" << std::endl;
            }
         }
      }
   }
}