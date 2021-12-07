#include <iostream>
#include <memory>
#include <vector>

#include "NamelessWindow/Event.hpp"
#include "NamelessWindow/EventQueue.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

int main() {
   std::vector<NLSWIN::KeyboardDeviceInfo> keyboards = NLSWIN::Keyboard::EnumerateKeyboards();

   for (auto keyboardInfo: keyboards) {
      std::cout << keyboardInfo.name << " " << keyboardInfo.platformSpecificIdentifier << std::endl;
   }

   NLSWIN::WindowProperties properties {};
   std::vector<NLSWIN::Monitor> monitors = NLSWIN::Window::EnumerateMonitors();
   NLSWIN::Monitor monitor               = monitors.at(0);
   properties.preferredMonitor           = monitor;
   properties.windowName                 = "Example window!";
   properties.mode                       = NLSWIN::WindowMode::WINDOWED;

   for (auto monitor: monitors) {
      std::cout << monitor.name << std::endl;
      std::cout << "Global Space: " << monitor.globalSpaceXCoord << ", " << monitor.globalSpaceYCoord
                << std::endl;
      std::cout << monitor.horzResolution << "x" << monitor.verticalResolution << std::endl;
   }

   NLSWIN::Window window(properties);
   NLSWIN::Keyboard keyboard(window);

   while (!window.RequestedClose()) {
      NLSWIN::EventQueue::GetOSEvents();
      while (keyboard.HasEvent()) {
         NLSWIN::Event nextEvent = keyboard.GetNextEvent();
         if (auto keyEvent = std::get_if<NLSWIN::KeyEvent>(&nextEvent)) {
            std::cout << "Key event occured!" << std::endl;
         }
      }

      while (window.HasEvent()) {
         NLSWIN::Event nextEvent = window.GetNextEvent();

         if (auto closeEvent = std::get_if<NLSWIN::WindowFocusedEvent>(&nextEvent)) {
            std::cout << "Focused!" << std::endl;
         }
      }
   }
}