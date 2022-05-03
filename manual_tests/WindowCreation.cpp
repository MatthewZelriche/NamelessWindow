#include <iostream>

#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Window.hpp"

/** Test basic multi-window creation with standard, non-standard & multi-monitor construction parameters. */
using namespace NLSWIN;

int main() {
   WindowProperties props;
   props.horzResolution = 200;
   props.vertResolution = 200;
   props.isUserResizable = false;
   props.xCoordinate = 1000;
   props.yCoordinate = 100;

   auto nonStandardWindow = Window::Create(props);
   auto standardWindow = Window::Create();
   nonStandardWindow->Show();
   standardWindow->Show();

   // Uncomment to avoid multimon testing.
   auto monitors = Window::EnumerateMonitors();
   WindowProperties multiMonProps;
   multiMonProps.preferredMonitor.emplace(monitors[1]);
   auto secondMonWindow = Window::Create(multiMonProps);
   secondMonWindow->Show();

   while (nonStandardWindow || standardWindow) {
      NLSWIN::EventBus::PollEvents();
      if (nonStandardWindow && nonStandardWindow->RequestedClose()) {
         nonStandardWindow.reset();
      }
      if (standardWindow && standardWindow->RequestedClose()) {
         standardWindow.reset();
      }
   }
}