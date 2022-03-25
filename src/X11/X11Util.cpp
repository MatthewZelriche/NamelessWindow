#include "X11Util.hpp"

#include <X11/extensions/Xrandr.h>
#include <cstring>

#include "NamelessWindow/Exceptions.hpp"
#include "XConnection.h"

xcb_screen_t *NLSWIN::UTIL::GetDefaultScreen() {
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

int NLSWIN::UTIL::GetDefaultScreenNumber() {
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
   return defaultScreenNum;
}

xcb_window_t NLSWIN::UTIL::GetRootWindow() {
   return GetDefaultScreen()->root;
}

XRRMonitorInfo *NLSWIN::UTIL::GetCurrentMonitor(xcb_window_t window, Point topRightWindowPos) {
   int numMonitors = 0;
   XRRMonitorInfo *monitorInfos = XRRGetMonitors(XConnection::GetDisplay(), window, true, &numMonitors);

   XRRMonitorInfo *currentMonitor = nullptr;
   XRRMonitorInfo *primaryMonitor = nullptr;
   for (int i = 0; i < numMonitors; i++) {
      // Store the primary monitor in case the given window isn't within the bounds of any monitor.
      if (monitorInfos[i].primary == true) {
         primaryMonitor = &monitorInfos[i];
      }
      if (UTIL::IsPointInRect({monitorInfos[i].x, monitorInfos[i].y, monitorInfos[i].width, monitorInfos[i].height}, {topRightWindowPos.x, topRightWindowPos.y})) {
         // Found the monitor that this window is in.
         currentMonitor = &monitorInfos[i];
      }
   }

   // If the window's topright corner isnt within any monitor, then we just select the primary monitor.
   if (!currentMonitor) {
      currentMonitor = primaryMonitor;
   }

   return currentMonitor;
}

bool NLSWIN::UTIL::IsPointInRect(Rect rectangle, Point position) {
   return (position.x >= rectangle.x && position.x <= rectangle.x + rectangle.width) && (position.y >= rectangle.y && position.y <= rectangle.y + rectangle.height);
}