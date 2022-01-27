#include "X11Window.hpp"

#include <xcb/randr.h>
#include <xcb/xcb_icccm.h>

#include <cstring>
#include <memory>

#include "NamelessWindow/Exceptions.hpp"
#include "X11EventBus.hpp"
#include "X11Util.hpp"
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
   m_defaultScreen = UTIL::GetDefaultScreen();
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

   // X defaults to windowed
   if (properties.mode == WindowMode::FULLSCREEN) {
      SetFullscreen(false);
   } else if (properties.mode == WindowMode::BORDERLESS) {
      SetFullscreen(true);
   }

   m_windowGeometry = GetNewGeometry();
   xcb_flush(XConnection::GetConnection());
   NewID();
}

X11Window::~X11Window() {
   xcb_destroy_window(XConnection::GetConnection(), m_x11WindowID);
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

void X11Window::SetFullscreen(bool borderless) noexcept {
   if (m_windowMode == WindowMode::FULLSCREEN || m_windowMode == WindowMode::BORDERLESS) {
      return;
   }
   ToggleFullscreen();
   m_windowMode = borderless ? WindowMode::BORDERLESS : WindowMode::FULLSCREEN;
}

void X11Window::SetWindowed() noexcept {
   if (m_windowMode == WindowMode::WINDOWED) {
      return;
   }
   ToggleFullscreen();
   m_windowMode = WindowMode::WINDOWED;
}

void X11Window::Reposition(uint32_t newX, uint32_t newY) noexcept {
   uint32_t newCoords[] = {newX, newY};
   xcb_configure_window(XConnection::GetConnection(), m_x11WindowID,
                        XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, newCoords);
   xcb_flush(XConnection::GetConnection());
}

void X11Window::Resize(uint32_t width, uint32_t height) noexcept {
   uint32_t newSize[] = {width, height};
   // Set the new desired width and height in case the wm doesn't respect it.
   m_preferredWidth = width;
   m_preferredHeight = height;
   xcb_configure_window(XConnection::GetConnection(), m_x11WindowID,
                        XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, newSize);

   m_width = width;
   m_height = height;
   xcb_flush(XConnection::GetConnection());
}

void X11Window::ToggleFullscreen() noexcept {
   bool fullScreen = 0;
   if (m_windowMode == WindowMode::FULLSCREEN || m_windowMode == WindowMode::BORDERLESS) {
      fullScreen = 0;
   } else {
      fullScreen = 1;
   }

   xcb_intern_atom_cookie_t stateCookie =
      xcb_intern_atom(XConnection::GetConnection(), 1, std::strlen("_NET_WM_STATE"), "_NET_WM_STATE");
   xcb_intern_atom_cookie_t fullscreenCookie = xcb_intern_atom(
      XConnection::GetConnection(), 1, std::strlen("_NET_WM_STATE_FULLSCREEN"), "_NET_WM_STATE_FULLSCREEN");

   xcb_client_message_event_t message {0};
   message.response_type = XCB_CLIENT_MESSAGE;

   xcb_intern_atom_reply_t *stateReply =
      xcb_intern_atom_reply(XConnection::GetConnection(), stateCookie, nullptr);
   xcb_intern_atom_reply_t *fullscreenReply =
      xcb_intern_atom_reply(XConnection::GetConnection(), fullscreenCookie, nullptr);

   message.type = stateReply->atom;
   message.format = 32;
   message.window = m_x11WindowID;
   message.data.data32[0] = fullScreen;
   message.data.data32[1] = fullscreenReply->atom;
   message.data.data32[2] = 0;

   xcb_send_event(XConnection::GetConnection(), true, m_x11WindowID,
                  XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                  (const char *)&message);

   xcb_flush(XConnection::GetConnection());
   free(stateReply);
   free(fullscreenReply);
}

void X11Window::ProcessGenericEvent(xcb_generic_event_t *event) {
   switch (event->response_type & ~0x80) {
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
               // Set new geometry
               m_windowGeometry = GetNewGeometry();
            }
            if (notifyEvent->x != m_windowGeometry.x || notifyEvent->y != m_windowGeometry.y) {
               m_windowGeometry = GetNewGeometry();
            }
         }
         break;
      }
      case XCB_FOCUS_IN: {
         xcb_focus_in_event_t *focusEvent = reinterpret_cast<xcb_focus_in_event_t *>(event);
         if (focusEvent->event == m_x11WindowID) {
            WindowFocusedEvent windowFocusEvent;
            windowFocusEvent.sourceWindow = m_genericID;
            PushEvent(windowFocusEvent);
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
      case XCB_CLIENT_MESSAGE: {
         // XCB_CLIENT_MESSAGE is currently only used for overriding the X11 window manager and handling
         // a close event directly. The close event is not sent to the API user, it is only handled
         // internally.
         xcb_client_message_event_t *clientEvent = reinterpret_cast<xcb_client_message_event_t *>(event);
         xcb_intern_atom_cookie_t deleteWindowCookie = xcb_intern_atom(
            XConnection::GetConnection(), false, std::strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
         xcb_intern_atom_reply_t *deleteWindowAtomReply =
            xcb_intern_atom_reply(XConnection::GetConnection(), deleteWindowCookie, nullptr);

         // Test if this is actually a close event.
         if (clientEvent->data.data32[0] == deleteWindowAtomReply->atom) {
            // No need to push anything. Just handle it internally!
            if (clientEvent->window == m_x11WindowID) {
               m_shouldClose = true;
            }
         }
         free(deleteWindowAtomReply);
         break;
      }
   }
}

Rect X11Window::GetNewGeometry() {
   auto geomCookie = xcb_get_geometry(XConnection::GetConnection(), m_x11WindowID);
   auto geomReply = xcb_get_geometry_reply(XConnection::GetConnection(), geomCookie, nullptr);
   return {geomReply->x, geomReply->y};
}

std::vector<MonitorInfo> NLSWIN::Window::EnumerateMonitors() {
   xcb_screen_t *defaultScreen = UTIL::GetDefaultScreen();
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