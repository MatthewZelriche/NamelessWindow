#include <iostream>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/EventDispatcher.hpp"
#include "NamelessWindow/Pointer.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   NLSWIN::WindowProperties props;
   props.isUserResizable = false;
   auto monitors = NLSWIN::Window::EnumerateMonitors();
   for (auto monitor: monitors) {
      if (monitor.name == "VIRTUAL-RIGHT") {
         props.preferredMonitor.emplace(monitor);
      }
   }
   std::shared_ptr<NLSWIN::Window> window = NLSWIN::Window::Create(props);
   std::shared_ptr<NLSWIN::Window> window2 = NLSWIN::Window::Create();

   // List enabled pointer devices.
   auto pointerInfos = NLSWIN::Pointer::EnumeratePointers();
   for (auto info: pointerInfos) { std::cout << info.name << std::endl; }
   // If you construct a raw pointer device object, you will automatically start recieving duplicate events
   // from the master pointer device. If you want to use a raw pointer device, you need to not use
   // the master pointer at all, unless you are okay with duplicate events.
   // std::shared_ptr<NLSWIN::Pointer> pointer = NLSWIN::Pointer::Create({"blah", 8}, window.get());
   NLSWIN::MasterPointer &pointer = NLSWIN::MasterPointer::GetMasterPointer();
   pointer.BindToWindow(window2.get());
   pointer.HideCursor();

   while (window || window2) {
      NLSWIN::EventDispatcher::GetOSEvents();

      if (window != nullptr && window->RequestedClose()) {
         window.reset();
      }
      if (window2 != nullptr && window2->RequestedClose()) {
         window2.reset();
      }

      while (pointer.HasEvent()) {
         NLSWIN::Event nextEvent = pointer.GetNextEvent();

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