#include <iostream>
#include <memory>
#include <vector>

#include "NamelessWindow/Window.hpp"

int main() {
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

   while (true)
      ;
}