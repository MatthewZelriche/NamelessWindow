#include "X11Window.hpp"

#include <xcb/randr.h>
#include <xcb/xcb_icccm.h>

#include <cstring>
#include <memory>

#include "NamelessWindow/Exceptions.hpp"
#include "X11EventBus.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::shared_ptr<NLSWIN::Window> NLSWIN::Window::Create() {
   std::shared_ptr<X11Window> impl = std::make_shared<X11Window>(WindowProperties());
   X11EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

std::shared_ptr<NLSWIN::Window> NLSWIN::Window::Create(WindowProperties properties) {
   std::shared_ptr<X11Window> impl = std::make_shared<X11Window>(properties);
   X11EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

X11Window::X11Window(WindowProperties properties) {
   // Currently, this library doesn't support multiple screens.
   m_defaultScreen = GetDefaultScreen();
   if (!m_defaultScreen) {
      throw PlatformInitializationException();
   }
   // TODO: Verify the monitor supplied is valid.

   m_rootWindow = m_defaultScreen->root;
   // If a specific monitor is specified, set the preferred window coordinates relative to the monitor.
   if (properties.preferredMonitor.has_value()) {
      if (properties.mode == WindowMode::WINDOWED) {
         m_preferredXCoord = properties.preferredMonitor.value().screenXCord + properties.xCoordinate;
         m_preferredYCoord = properties.preferredMonitor.value().screenYCord + properties.yCoordinate;
      } else {
         m_preferredXCoord = properties.preferredMonitor.value().screenXCord;
         m_preferredYCoord = properties.preferredMonitor.value().screenYCord;
      }
   } else {
      m_preferredXCoord = properties.xCoordinate;
      m_preferredYCoord = properties.yCoordinate;
   }

   m_preferredWidth = properties.horzResolution;
   m_preferredHeight = properties.vertResolution;

   // Set preferred border width for windowed applications
   if (properties.mode == WindowMode::WINDOWED) {
      m_preferredBorderWidth = properties.borderWidth;
   }

   std::array<uint32_t, 1> valueMaskArray;
   valueMaskArray[0] = m_eventMask;
   m_x11WindowID = xcb_generate_id(XConnection::GetConnection());
   // TODO: Look into visual. Look into masks.
   xcb_create_window(XConnection::GetConnection(), XCB_COPY_FROM_PARENT, m_x11WindowID, m_rootWindow,
                     m_preferredXCoord, m_preferredYCoord, m_preferredWidth, m_preferredHeight,
                     m_preferredBorderWidth, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT,
                     XCB_CW_EVENT_MASK, valueMaskArray.data());

   if (!properties.isUserResizable) {
      DisableUserResizing();
   }
   auto geomCookie = xcb_get_geometry(XConnection::GetConnection(), m_x11WindowID);

   // Set window name if we were given one.
   if (!properties.windowName.empty()) {
      xcb_change_property(XConnection::GetConnection(), XCB_PROP_MODE_REPLACE, m_x11WindowID,
                          XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, properties.windowName.length(),
                          properties.windowName.c_str());
   }

   // Redirect window close events to the application.
   xcb_intern_atom_cookie_t protocolsCookie =
      xcb_intern_atom(XConnection::GetConnection(), false, std::strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
   xcb_intern_atom_cookie_t deleteWindowCookie = xcb_intern_atom(
      XConnection::GetConnection(), false, std::strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
   xcb_intern_atom_reply_t *protocolsAtomReply =
      xcb_intern_atom_reply(XConnection::GetConnection(), protocolsCookie, nullptr);
   xcb_intern_atom_reply_t *deleteWindowAtomReply =
      xcb_intern_atom_reply(XConnection::GetConnection(), deleteWindowCookie, nullptr);
   xcb_change_property(XConnection::GetConnection(), XCB_PROP_MODE_REPLACE, m_x11WindowID,
                       protocolsAtomReply->atom, XCB_ATOM_ATOM, 32, 1, &(deleteWindowAtomReply->atom));
   free(protocolsAtomReply);
   free(deleteWindowAtomReply);

   // TODO: Set Window Mode

   SubscribeToEvents(m_eventMask);
   xcb_flush(XConnection::GetConnection());
}

void X11Window::DisableUserResizing() {
   xcb_size_hints_t hints;
   // Setting the min and max size to the same values ensures the window cant be resized - assuming window
   // managers respect this.
   xcb_icccm_size_hints_set_min_size(&hints, m_preferredWidth, m_preferredHeight);
   xcb_icccm_size_hints_set_max_size(&hints, m_preferredWidth, m_preferredHeight);

   xcb_icccm_set_wm_size_hints(XConnection::GetConnection(), m_x11WindowID, XCB_ATOM_WM_NORMAL_HINTS, &hints);
}
void X11Window::EnableUserResizing() {
   xcb_size_hints_t hints;
   // Setting the min and max size to the same values ensures the window cant be resized - assuming window
   // managers respect this.
   xcb_icccm_size_hints_set_min_size(&hints, 0, 0);
   xcb_icccm_size_hints_set_max_size(&hints, INT32_MAX, INT32_MAX);  // TODO: What's a good max size thats
                                                                     // cross-platform?

   xcb_icccm_set_wm_size_hints(XConnection::GetConnection(), m_x11WindowID, XCB_ATOM_WM_NORMAL_HINTS, &hints);
}

void X11Window::Show() {
   xcb_map_window(XConnection::GetConnection(), m_x11WindowID);
   free(xcb_get_input_focus_reply(XConnection::GetConnection(),
                                  xcb_get_input_focus(XConnection::GetConnection()), NULL));
   // Block until we've actually been mapped.
   while (!m_isMapped) { X11EventBus::GetInstance().PollEvents(); }
}
void X11Window::Hide() {
   xcb_unmap_window(XConnection::GetConnection(), m_x11WindowID);
   free(xcb_get_input_focus_reply(XConnection::GetConnection(),
                                  xcb_get_input_focus(XConnection::GetConnection()), NULL));
   while (m_isMapped) { X11EventBus::GetInstance().PollEvents(); }
}

void X11Window::ProcessGenericEvent(xcb_generic_event_t *event) {
   switch (event->response_type & ~0x80) {
      case XCB_CONFIGURE_NOTIFY: {
         xcb_configure_notify_event_t *notify = (xcb_configure_notify_event_t *)event;
         if (m_width != notify->width || m_height != notify->height) {
            m_width = notify->width;
            m_height = notify->height;
         }
         break;
      }
      case XCB_MAP_NOTIFY: {
         m_isMapped = true;
         break;
      }
      case XCB_UNMAP_NOTIFY: {
         m_isMapped = false;
         break;
      }
   }
}

std::vector<MonitorInfo> NLSWIN::Window::EnumerateMonitors() {
   xcb_screen_t *defaultScreen = X11Window::GetDefaultScreen();
   if (!defaultScreen) {
      return {};
   }
   // Fill the list with monitor info structs.
   std::vector<MonitorInfo> listOfMonitors;
   auto monitorsCookie = xcb_randr_get_monitors(XConnection::GetConnection(), defaultScreen->root, true);
   auto monitorsReply = xcb_randr_get_monitors_reply(XConnection::GetConnection(), monitorsCookie, nullptr);
   auto monitorIter = xcb_randr_get_monitors_monitors_iterator(monitorsReply);
   while (monitorIter.rem > 0) {
      char *monitorName = xcb_get_atom_name_name(xcb_get_atom_name_reply(
         XConnection::GetConnection(),
         xcb_get_atom_name(XConnection::GetConnection(), monitorIter.data->name), nullptr));
      MonitorInfo monitor {monitorIter.data->width, monitorIter.data->height, monitorIter.data->x,
                           monitorIter.data->y, monitorName};
      listOfMonitors.push_back(monitor);
      xcb_randr_monitor_info_next(&monitorIter);
   }

   free(monitorsReply);
   return listOfMonitors;
}

xcb_screen_t *NLSWIN::X11Window::GetDefaultScreen() {
   // Quick side connection with xcb to get the default screen number - our main connection cant do that.
   int defaultScreenNum = 0;
   auto connection = xcb_connect(nullptr, &defaultScreenNum);
   if (!connection) {
      throw PlatformInitializationException();
   }

   // Get the default screen.
   xcb_screen_t *defaultScreen = nullptr;
   auto screenIter = xcb_setup_roots_iterator(xcb_get_setup(connection));
   while (screenIter.rem > 0) {
      if (defaultScreenNum == 0) {
         defaultScreen = screenIter.data;
         break;
      }
      defaultScreenNum -= 1;
      xcb_screen_next(&screenIter);
   }

   xcb_disconnect(connection);
   return defaultScreen;
}