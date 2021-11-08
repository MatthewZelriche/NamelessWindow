#include "WindowX11.hpp"

#include <xcb/randr.h>
#include <xcb/xcb.h>

#include <memory>

#include "NamelessWindow/exceptions.hpp"

using namespace NLSWIN;

std::unique_ptr<Window> Window::CreateWindow() {
   return std::make_unique<WindowX11>();
}

std::vector<Screen> Window::EnumerateScreens() {
   std::vector<Screen> listOfScreens;
   // Open a brief temporary connection to get the screens
   xcb_connection_t *connection = xcb_connect(nullptr, nullptr);
   int result                   = xcb_connection_has_error(connection);
   if (result != 0) {
      xcb_disconnect(connection);
      return {};
   }

   auto screenIter = xcb_setup_roots_iterator(xcb_get_setup(connection));
   do {
      Screen screen {};
      auto screenInfo = xcb_randr_get_screen_info_reply(
         connection, xcb_randr_get_screen_info_unchecked(connection, screenIter.data->root), nullptr);

      screen.horzResolution     = screenIter.data->width_in_pixels;
      screen.verticalResolution = screenIter.data->height_in_pixels;
      auto monitorsReply        = xcb_randr_get_monitors_reply(
         connection, xcb_randr_get_monitors(connection, screenIter.data->root, true), nullptr);

      auto monitorIter = xcb_randr_get_monitors_monitors_iterator(monitorsReply);
      do {
         // Get name of the current monitor.
         char *monitorName = xcb_get_atom_name_name(xcb_get_atom_name_reply(
            connection, xcb_get_atom_name(connection, monitorIter.data->name), nullptr));
         screen.name       = monitorName;
         screen.xCoord     = monitorIter.data->x;
         screen.yCoord     = monitorIter.data->y;

         // Copying a new screen struct into the vector for each different monitor.
         listOfScreens.push_back(screen);
         xcb_randr_monitor_info_next(&monitorIter);
      } while (monitorIter.rem > 0);

      free(screenInfo);
      free(monitorsReply);
      xcb_screen_next(&screenIter);
   } while (screenIter.rem > 0);

   xcb_disconnect(connection);
   return listOfScreens;
}

WindowX11::WindowX11() {
   int preferredScreenNum = 0;
   m_xServerConnection    = xcb_connect(nullptr, &preferredScreenNum);
   int result             = xcb_connection_has_error(m_xServerConnection);
   if (result != 0) {
      throw std::runtime_error("Unspecified error attempting to establish display server connection");
   }
}

int WindowX11::SetSomething(int value) {
   return value * 3;
}