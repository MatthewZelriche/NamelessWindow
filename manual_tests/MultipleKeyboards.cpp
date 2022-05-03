#include <iostream>

#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

/** Test multiple physical keyboards with more than one window. */
using namespace NLSWIN;

int main() {
   WindowProperties props;
   props.xCoordinate = 1000;
   props.yCoordinate = 0;

   auto nonStandardWindow = Window::Create(props);
   auto standardWindow = Window::Create();
   nonStandardWindow->Show();
   standardWindow->Show();

   auto keyboards = Keyboard::EnumerateKeyboards();
   for (auto keyboard: keyboards) {
      std::cout << keyboard.name << ", " << keyboard.platformSpecificIdentifier << std::endl;
   }
   auto kb = Keyboard::Create(keyboards[0]);
   auto kb2 = Keyboard::Create(keyboards[1]);
   kb->SubscribeToWindow(standardWindow);
   kb->SubscribeToWindow(nonStandardWindow);

   kb2->SubscribeToWindow(standardWindow);

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
            std::cout << "Hello from PS/2 keyboard, window " << keyEvent->sourceWindow << "! Key " << keyEvent->keyName
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

      while (kb2->HasEvent()) {
         auto event = kb2->GetNextEvent();
         if (auto keyEvent = std::get_if<KeyEvent>(&event)) {
            std::cout << "Hello from USB keyboard, window " << keyEvent->sourceWindow << "! Key " << keyEvent->keyName
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