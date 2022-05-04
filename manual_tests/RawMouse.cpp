#include "NamelessWindow/RawMouse.hpp"

#include <iostream>

#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Window.hpp"

/** Test raw mouse input events. */
using namespace NLSWIN;

int main() {
   WindowProperties props;
   props.xCoordinate = 1000;
   props.yCoordinate = 0;

   auto nonStandardWindow = Window::Create(props);
   auto standardWindow = Window::Create();
   nonStandardWindow->Show();
   standardWindow->Show();

   auto mice = RawMouse::EnumeratePointers();
   for (auto mouse: mice) {
      std::cout << mouse.name << ", " << mouse.platformSpecificIdentifier << std::endl;
   }

   // Edit for different mouse configs.
   auto rawMouse = RawMouse::Create(mice[0]);

   while (nonStandardWindow || standardWindow) {
      NLSWIN::EventBus::PollEvents();
      if (nonStandardWindow && nonStandardWindow->RequestedClose()) {
         nonStandardWindow.reset();
      }
      if (standardWindow && standardWindow->RequestedClose()) {
         standardWindow.reset();
      }

      while (rawMouse->HasEvent()) { 
          auto event = rawMouse->GetNextEvent();

          if (auto mouseEvent = std::get_if<RawMouseButtonEvent>(&event)) {
             std::cout << "Raw mouse button " << (int)mouseEvent->button << " with press type " << (int)mouseEvent->type
                       << std::endl;
          } else if (auto mouseEvent = std::get_if<RawMouseScrollEvent>(&event)) {
             std::cout << "Raw mouse scroll " << (int)mouseEvent->scrollType << std::endl;
          } else if (auto mouseEvent = std::get_if<RawMouseDeltaMovementEvent>(&event)) {
             std::cout << "Raw mouse movement, X = " << mouseEvent->deltaX << ", Y = " << mouseEvent->deltaY
                       << std::endl;
          }
      }
      
   }
}