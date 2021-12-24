#include "Window.x11.hpp"

#include <xcb/randr.h>

#include <cstring>
#include <memory>

#include "EventQueue.x11.hpp"
#include "Keyboard.x11.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "XConnection.h"

using namespace NLSWIN;

xcb_connection_t *Window::Impl::m_xServerConnection = nullptr;

Window::Impl::Impl(WindowProperties properties, const Window &window) {
   XConnection::CreateConnection();
   m_xServerConnection = XConnection::GetConnection();

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
   valueMaskArray[0] = m_eventMask;
   // Fetch a new window from X11.
   m_x11WindowID = xcb_generate_id(m_xServerConnection);
   // TODO: Look into visual. Look into masks.
   xcb_create_window(m_xServerConnection, XCB_COPY_FROM_PARENT, m_x11WindowID, preferredScreen->root,
                     windowXCoord, windowYCoord, properties.horzResolution, properties.vertResolution,
                     borderWidth, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT, XCB_CW_EVENT_MASK,
                     valueMaskArray.data());

   auto geomCookie = xcb_get_geometry(m_xServerConnection, m_x11WindowID);

   // Set window name if we were given one.
   if (!properties.windowName.empty()) {
      xcb_change_property(m_xServerConnection, XCB_PROP_MODE_REPLACE, m_x11WindowID, XCB_ATOM_WM_NAME,
                          XCB_ATOM_STRING, 8, properties.windowName.length(), properties.windowName.c_str());
   }

   // Redirect window close events to the application.
   xcb_intern_atom_cookie_t protocolsCookie =
      xcb_intern_atom(m_xServerConnection, false, std::strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
   xcb_intern_atom_cookie_t deleteWindowCookie =
      xcb_intern_atom(m_xServerConnection, false, std::strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
   xcb_intern_atom_reply_t *protocolsAtomReply =
      xcb_intern_atom_reply(m_xServerConnection, protocolsCookie, nullptr);
   xcb_intern_atom_reply_t *deleteWindowAtomReply =
      xcb_intern_atom_reply(m_xServerConnection, deleteWindowCookie, nullptr);
   xcb_change_property(m_xServerConnection, XCB_PROP_MODE_REPLACE, m_x11WindowID, protocolsAtomReply->atom,
                       XCB_ATOM_ATOM, 32, 1, &(deleteWindowAtomReply->atom));

   // Present
   xcb_map_window(m_xServerConnection, m_x11WindowID);

   // X defaults to windowed, so if we want to initialize as fullscreen, just toggle.
   if (properties.mode == WindowMode::FULLSCREEN) {
      ToggleFullscreen();
      m_currentWindowMode = WindowMode::FULLSCREEN;
   } else if (properties.mode == WindowMode::BORDERLESS) {
      ToggleFullscreen();
      m_currentWindowMode = WindowMode::BORDERLESS;
   }

   // We need to get the actual width and height, not what we preferably would set it to, since
   // our preferred resolution may not have been respected.
   auto geomCookieReply = xcb_get_geometry_reply(m_xServerConnection, geomCookie, nullptr);
   m_width = geomCookieReply->width;
   m_height = geomCookieReply->height;
   free(geomCookieReply);

   xcb_flush(m_xServerConnection);
}

void Window::Impl::SetFullscreen(bool borderless) noexcept {
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

void Window::Impl::SetWindowed() noexcept {
   if (m_currentWindowMode == WindowMode::WINDOWED) {
      return;
   }

   ToggleFullscreen();

   m_currentWindowMode = WindowMode::WINDOWED;
}

void Window::Impl::Close() noexcept {
   receivedTerminateSignal = true;
}

void Window::Impl::ToggleFullscreen() noexcept {
   bool fullScreen = 0;
   if (m_currentWindowMode == WindowMode::FULLSCREEN || m_currentWindowMode == WindowMode::BORDERLESS) {
      fullScreen = 0;
   } else {
      fullScreen = 1;
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

   message.type = stateReply->atom;
   message.format = 32;
   message.window = m_x11WindowID;
   message.data.data32[0] = fullScreen;
   message.data.data32[1] = fullscreenReply->atom;
   message.data.data32[2] = 0;

   xcb_send_event(m_xServerConnection, true, m_x11WindowID,
                  XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                  (const char *)&message);

   xcb_flush(m_xServerConnection);
   free(stateReply);
   free(fullscreenReply);
}

xcb_screen_t *Window::Impl::GetScreenFromMonitor(Monitor monitor) const {
   auto screenIter = xcb_setup_roots_iterator(xcb_get_setup(m_xServerConnection));
   do {
      auto monitorsReply = xcb_randr_get_monitors_reply(
         m_xServerConnection, xcb_randr_get_monitors(m_xServerConnection, screenIter.data->root, true),
         nullptr);
      auto monitorIter = xcb_randr_get_monitors_monitors_iterator(monitorsReply);
      do {
         auto nameReply = xcb_get_atom_name_reply(
            m_xServerConnection, xcb_get_atom_name(m_xServerConnection, monitorIter.data->name), nullptr);
         char *monitorName = xcb_get_atom_name_name(nameReply);
         auto nameLen = xcb_get_atom_name_name_length(nameReply);

         if (std::strncmp(monitor.name.c_str(), monitorName, nameLen) == 0) {
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

std::vector<Monitor> Window::EnumerateMonitors() noexcept {
   std::vector<Monitor> listOfMonitors;
   // Open a brief temporary connection to get the screens
   xcb_connection_t *connection = xcb_connect(nullptr, nullptr);
   int result = xcb_connection_has_error(connection);
   if (result != 0) {
      xcb_disconnect(connection);
      return {};
   }

   auto screenIter = xcb_setup_roots_iterator(xcb_get_setup(connection));
   do {
      auto screenInfo = xcb_randr_get_screen_info_reply(
         connection, xcb_randr_get_screen_info_unchecked(connection, screenIter.data->root), nullptr);

      auto monitorsReply = xcb_randr_get_monitors_reply(
         connection, xcb_randr_get_monitors(connection, screenIter.data->root, true), nullptr);

      auto monitorIter = xcb_randr_get_monitors_monitors_iterator(monitorsReply);
      do {
         // Get name of the current monitor.
         char *monitorName = xcb_get_atom_name_name(xcb_get_atom_name_reply(
            connection, xcb_get_atom_name(connection, monitorIter.data->name), nullptr));
         Monitor monitor {monitorIter.data->width, monitorIter.data->height, monitorIter.data->x,
                          monitorIter.data->y, monitorName};

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

void Window::Impl::ProcessGenericEvent(xcb_generic_event_t *event) {
   switch (event->response_type & ~0x80) {
      // Handle resize events.
      case XCB_CONFIGURE_NOTIFY: {
         xcb_configure_notify_event_t *notifyEvent = reinterpret_cast<xcb_configure_notify_event_t *>(event);
         if (notifyEvent->window == m_x11WindowID) {
            // Has the window size changed?
            if (notifyEvent->width != m_width || notifyEvent->height != m_height) {
               m_width = notifyEvent->width;
               m_height = notifyEvent->height;
               WindowResizeEvent resizeEvent;
               resizeEvent.newWidth = m_width;
               resizeEvent.newHeight = m_height;
               PushEvent(resizeEvent);
            }
         }
      }
      case XCB_FOCUS_IN: {
         xcb_focus_in_event_t *focusEvent = reinterpret_cast<xcb_focus_in_event_t *>(event);
         if (focusEvent->event == m_x11WindowID) {
            WindowFocusedEvent windowFocusEvent;
            PushEvent(windowFocusEvent);
         }
         break;
      }
      case XCB_CLIENT_MESSAGE: {
         // XCB_CLIENT_MESSAGE is currently only used for overriding the X11 window manager and handling
         // a close event directly. The close event is not sent to the API user, it is only handled
         // internally.
         xcb_client_message_event_t *clientEvent = reinterpret_cast<xcb_client_message_event_t *>(event);
         xcb_intern_atom_cookie_t deleteWindowCookie =
            xcb_intern_atom(m_xServerConnection, false, std::strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
         xcb_intern_atom_reply_t *deleteWindowAtomReply =
            xcb_intern_atom_reply(m_xServerConnection, deleteWindowCookie, nullptr);

         // Test if this is actually a close event.
         if (clientEvent->data.data32[0] == deleteWindowAtomReply->atom) {
            // No need to push anything. Just handle it internally!
            if (clientEvent->window == m_x11WindowID) {
               Close();
            }
            break;
         }
      }
   }
}

Window::Window() : Window(WindowProperties()) {
}

Window::Window(WindowProperties properties) : m_pImpl(std::make_shared<Impl>(properties, *this)) {
   EventQueueX11::RegisterListener(m_pImpl);
}

Window::~Window() = default;

bool Window::HasEvent() const noexcept {
   return m_pImpl->HasEvent();
}

Event Window::GetNextEvent() {
   return m_pImpl->GetNextEvent();
}

void Window::SetFullscreen(bool borderless) noexcept {
   m_pImpl->SetFullscreen(borderless);
}

void Window::SetWindowed() noexcept {
   m_pImpl->SetWindowed();
}

WindowMode Window::GetWindowMode() const noexcept {
   return m_pImpl->GetWindowMode();
}

WindowID Window::GetWindowID() const noexcept {
   return m_pImpl->GetWindowID();
}

void Window::Close() noexcept {
   return m_pImpl->Close();
}

bool Window::RequestedClose() const noexcept {
   return m_pImpl->RequestedClose();
}
