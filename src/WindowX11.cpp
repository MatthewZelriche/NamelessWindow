#include "WindowX11.hpp"

#include <xcb/randr.h>
#include <xcb/xcb.h>

#include <memory>

#include "NamelessWindow/exceptions.hpp"

using namespace NLSWIN;

std::unique_ptr<Window> Window::CreateWindow(WindowProperties properties) {
   return std::make_unique<WindowX11>(properties);
}

#include <iostream>
WindowX11::WindowX11(WindowProperties properties) {
   int preferredScreenNum = 0;
   m_xServerConnection    = xcb_connect(nullptr, &preferredScreenNum);
   int result             = xcb_connection_has_error(m_xServerConnection);
   if (result != 0) {
      throw std::runtime_error("Unspecified error attempting to establish display server connection");
   }

   // Get the correct screen.
   xcb_screen_t *preferredScreen = nullptr;
   if (properties.preferredMonitor.has_value()) {
      xcb_screen_t *screenResult = GetScreenFromMonitor(properties.preferredMonitor.value());
      if (!screenResult) {
         throw BadMonitorException();
      } else {
         preferredScreen = screenResult;
      }
   } else {
      auto screenIter = xcb_setup_roots_iterator(xcb_get_setup(m_xServerConnection));
      for (int i = 0; i < preferredScreenNum; i++) { xcb_screen_next(&screenIter); }
      preferredScreen = screenIter.data;
   }
   if (!preferredScreen) {
      throw std::runtime_error("Unspecified error attempting to select a screen");
   }

   uint32_t windowXCoord = 0;
   uint32_t windowYCoord = 0;
   // If a specific monitor is specified, we must draw the window within that monitor's region.
   if (properties.preferredMonitor.has_value()) {
      if (properties.mode == WindowMode::WINDOWED) {
         windowXCoord = properties.preferredMonitor.value().globalSpaceXCoord + properties.xCoordinate;
         windowYCoord = properties.preferredMonitor.value().globalSpaceYCoord + properties.yCoordinate;
      } else {
         windowXCoord = properties.preferredMonitor.value().globalSpaceXCoord;
         windowYCoord = properties.preferredMonitor.value().globalSpaceYCoord;
      }
   } else {
      windowXCoord = properties.xCoordinate;
      windowYCoord = properties.yCoordinate;
   }

   // TODO: Fetch a new window from X11.
}

xcb_screen_t *WindowX11::GetScreenFromMonitor(Monitor monitor) {
   auto screenIter = xcb_setup_roots_iterator(xcb_get_setup(m_xServerConnection));
   do {
      auto monitorsReply = xcb_randr_get_monitors_reply(
         m_xServerConnection, xcb_randr_get_monitors(m_xServerConnection, screenIter.data->root, true),
         nullptr);
      auto monitorIter = xcb_randr_get_monitors_monitors_iterator(monitorsReply);
      do {
         char *monitorName = xcb_get_atom_name_name(xcb_get_atom_name_reply(
            m_xServerConnection, xcb_get_atom_name(m_xServerConnection, monitorIter.data->name), nullptr));
         if (monitor.name.compare(monitorName) == 0) {
            return screenIter.data;
         }
         xcb_randr_monitor_info_next(&monitorIter);
      } while (monitorIter.rem > 0);
      free(monitorsReply);
      xcb_screen_next(&screenIter);
   } while (screenIter.rem > 0);

   // Could not find the correct monitor!
   return nullptr;
}

std::vector<Monitor> Window::EnumerateMonitors() {
   std::vector<Monitor> listOfMonitors;
   // Open a brief temporary connection to get the screens
   xcb_connection_t *connection = xcb_connect(nullptr, nullptr);
   int result                   = xcb_connection_has_error(connection);
   if (result != 0) {
      xcb_disconnect(connection);
      return {};
   }

   auto screenIter = xcb_setup_roots_iterator(xcb_get_setup(connection));
   do {
      Monitor monitor {};
      auto screenInfo = xcb_randr_get_screen_info_reply(
         connection, xcb_randr_get_screen_info_unchecked(connection, screenIter.data->root), nullptr);

      auto monitorsReply = xcb_randr_get_monitors_reply(
         connection, xcb_randr_get_monitors(connection, screenIter.data->root, true), nullptr);

      auto monitorIter = xcb_randr_get_monitors_monitors_iterator(monitorsReply);
      do {
         // Get name of the current monitor.
         char *monitorName          = xcb_get_atom_name_name(xcb_get_atom_name_reply(
            connection, xcb_get_atom_name(connection, monitorIter.data->name), nullptr));
         monitor.name               = monitorName;
         monitor.globalSpaceXCoord  = monitorIter.data->x;
         monitor.globalSpaceYCoord  = monitorIter.data->y;
         monitor.horzResolution     = monitorIter.data->width;
         monitor.verticalResolution = monitorIter.data->height;

         // Copying a new screen struct into the vector for each different monitor.
         listOfMonitors.push_back(monitor);
         xcb_randr_monitor_info_next(&monitorIter);
      } while (monitorIter.rem > 0);

      free(screenInfo);
      free(monitorsReply);
      xcb_screen_next(&screenIter);
   } while (screenIter.rem > 0);

   xcb_disconnect(connection);
   return listOfMonitors;
}

int WindowX11::SetSomething(int value) {
   return value * 3;
}