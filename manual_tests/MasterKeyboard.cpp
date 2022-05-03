#include <iostream>

#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

/** Test a single, master keyboard instance with more than one window. */
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

   while (nonStandardWindow || standardWindow) {
      NLSWIN::EventBus::PollEvents();
      if (nonStandardWindow && nonStandardWindow->RequestedClose()) {
         nonStandardWindow.reset();
      }
      if (standardWindow && standardWindow->RequestedClose()) {
         standardWindow.reset();
      }

      while (kb->HasEvent()) {
         auto event = kb->GetNextEvent();
         if (auto keyEvent = std::get_if<KeyEvent>(&event)) {
            std::cout << "Hello from window " << keyEvent->sourceWindow << "! Key " << keyEvent->keyName
                      << " has press type " << (int)keyEvent->pressType
                      << ". Ctrl: " << keyEvent->code.modifiers.ctrl
                      << ", Alt: " << keyEvent->code.modifiers.alt
                      << ", Shift: " << keyEvent->code.modifiers.shift
                      << ", Super: " << keyEvent->code.modifiers.super
                      << ", Caps: " << keyEvent->code.modifiers.capsLock
                      << ", Num: " << keyEvent->code.modifiers.numLock
                      << ", Scroll: " << keyEvent->code.modifiers.scrollLock << std::endl;
         }
      }
   }
}