#include "NamelessWindow/Cursor.hpp"

#include <iostream>

#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

/** Test cursor events and manipulation methods. */
using namespace NLSWIN;

int main() {
   WindowProperties props;
   props.xCoordinate = 1000;
   props.yCoordinate = 0;

   auto nonStandardWindow = Window::Create(props);
   auto standardWindow = Window::Create();
   nonStandardWindow->Show();
   standardWindow->Show();

   auto kb = Keyboard::Create();
   kb->SubscribeToWindow(standardWindow);
   kb->SubscribeToWindow(nonStandardWindow);
   auto cursor = Cursor::Create();

   while (nonStandardWindow || standardWindow) {
      NLSWIN::EventBus::PollEvents();
      if (nonStandardWindow && nonStandardWindow->RequestedClose()) {
         nonStandardWindow.reset();
      }
      if (standardWindow && standardWindow->RequestedClose()) {
         standardWindow.reset();
      }

      while (cursor->HasEvent()) {
         auto event = cursor->GetNextEvent();

         if (auto mouseEvent = std::get_if<MouseButtonEvent>(&event)) {
            std::cout << "Window " << mouseEvent->sourceWindow << " button " << (int)mouseEvent->button
                      << " with press type " << (int)mouseEvent->type << " at coords " << mouseEvent->xPos
                      << ", " << mouseEvent->yPos << std::endl;
         } else if (auto mouseEvent = std::get_if<MouseScrollEvent>(&event)) {
            std::cout << "Window " << mouseEvent->sourceWindow << " scroll " << (int)mouseEvent->scrollType
                      << " at coords " << mouseEvent->xPos << ", " << mouseEvent->yPos << std::endl;
         } else if (auto mouseEvent = std::get_if<RawMouseDeltaMovementEvent>(&event)) {
            // std::cout << "Raw mouse movement, X = " << mouseEvent->deltaX << ", Y = " << mouseEvent->deltaY
            //           << std::endl;
         } else if (auto mouseEvent = std::get_if<MouseMovementEvent>(&event)) {
            std::cout << "[Window " << mouseEvent->sourceWindow
                      << "]  Mouse is now at, X = " << mouseEvent->newXPos << ", Y = " << mouseEvent->newYPos
                      << std::endl;
         }
      }

      while (kb->HasEvent()) {
         auto event = kb->GetNextEvent();
         if (auto kbEvent = std::get_if<KeyEvent>(&event)) {
            switch (kbEvent->code.value) {
               case KEY_1: {
                  cursor->BindToWindow(standardWindow.get());
                  break;
               }
               case KEY_2: {
                  cursor->UnbindFromWindows();
                  break;
               }
               case KEY_3: {
                  cursor->Hide();
                  break;
               }
               case KEY_4: {
                  cursor->Show();
                  break;
               }
            }
         }
      }
   }
}