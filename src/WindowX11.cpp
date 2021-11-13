#include "WindowX11.hpp"

#include <xcb/randr.h>

#include <cstring>
#include <memory>

#include "NamelessWindow/exceptions.hpp"

using namespace NLSWIN;

xcb_connection_t *WindowX11::m_xServerConnection = nullptr;

std::unique_ptr<Window> Window::CreateWindow(WindowProperties properties) {
   return std::make_unique<WindowX11>(properties);
}

WindowX11::WindowX11(WindowProperties properties) {
   // If we haven't opened a connection, do so now.
   if (!m_xServerConnection) {
      m_preferredScreenNum = 0;
      m_xServerConnection  = xcb_connect(nullptr, &m_preferredScreenNum);
      int result           = xcb_connection_has_error(m_xServerConnection);
      if (result != 0) {
         throw std::runtime_error("Unspecified error attempting to establish display server connection");
      }
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
      for (int i = 0; i < m_preferredScreenNum; i++) { xcb_screen_next(&screenIter); }
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

   uint16_t borderWidth = 0;
   // Set border width for windowed applications
   if (properties.mode == WindowMode::WINDOWED) {
      borderWidth = properties.borderWidth;
   }

   std::array<uint32_t, 1> valueMaskArray;
   valueMaskArray[0] = XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;
   // Fetch a new window from X11.
   m_x11WindowID = xcb_generate_id(m_xServerConnection);
   // TODO: Look into visual. Look into masks.
   xcb_create_window(m_xServerConnection, XCB_COPY_FROM_PARENT, m_x11WindowID, preferredScreen->root,
                     windowXCoord, windowYCoord, properties.horzResolution, properties.vertResolution,
                     borderWidth, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT, XCB_CW_EVENT_MASK,
                     valueMaskArray.data());

   // Set window name if we were given one.
   if (!properties.windowName.empty()) {
      xcb_change_property(m_xServerConnection, XCB_PROP_MODE_REPLACE, m_x11WindowID, XCB_ATOM_WM_NAME,
                          XCB_ATOM_STRING, 8, properties.windowName.length(), properties.windowName.c_str());
   }

   // Present
   xcb_map_window(m_xServerConnection, m_x11WindowID);

   if (properties.mode == WindowMode::FULLSCREEN) {
      SetFullscreen(false);
   } else if (properties.mode == WindowMode::WINDOWED) {
      SetWindowed();
   } else {
      SetFullscreen(true);
   }

   // Flush
   xcb_flush(m_xServerConnection);
}

void WindowX11::SetFullscreen(bool borderless) {
   if (m_currentWindowMode == WindowMode::FULLSCREEN || m_currentWindowMode == WindowMode::BORDERLESS) {
      return;
   }

   ToggleFullscreen();

   if (borderless) {
      m_currentWindowMode = WindowMode::BORDERLESS;
   } else {
      m_currentWindowMode = WindowMode::FULLSCREEN;
   }
}

void WindowX11::SetWindowed() {
   if (m_currentWindowMode == WindowMode::WINDOWED) {
      return;
   }

   ToggleFullscreen();

   m_currentWindowMode = WindowMode::WINDOWED;
}

void WindowX11::ToggleFullscreen() {
   bool fullScreen = 0;
   if (m_currentWindowMode == WindowMode::FULLSCREEN || m_currentWindowMode == WindowMode::BORDERLESS) {
      fullScreen = 1;
   } else {
      fullScreen = 0;
   }

   xcb_intern_atom_cookie_t stateCookie =
      xcb_intern_atom(m_xServerConnection, 1, std::strlen("_NET_WM_STATE"), "_NET_WM_STATE");
   xcb_intern_atom_cookie_t fullscreenCookie = xcb_intern_atom(
      m_xServerConnection, 1, std::strlen("_NET_WM_STATE_FULLSCREEN"), "_NET_WM_STATE_FULLSCREEN");

   xcb_client_message_event_t message {0};
   message.response_type = XCB_CLIENT_MESSAGE;

   xcb_intern_atom_reply_t *stateReply = xcb_intern_atom_reply(m_xServerConnection, stateCookie, nullptr);
   xcb_intern_atom_reply_t *fullscreenReply =
      xcb_intern_atom_reply(m_xServerConnection, fullscreenCookie, nullptr);

   message.type           = stateReply->atom;
   message.format         = 32;
   message.window         = m_x11WindowID;
   message.data.data32[0] = fullScreen;
   message.data.data32[1] = fullscreenReply->atom;
   message.data.data32[2] = 0;

   xcb_send_event(m_xServerConnection, true, m_x11WindowID,
                  XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                  (const char *)&message);

   free(stateReply);
   free(fullscreenReply);
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