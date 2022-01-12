#include "X11Util.hpp"

#include "NamelessWindow/Exceptions.hpp"

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

xcb_window_t NLSWIN::UTIL::GetRootWindow() {
   return GetDefaultScreen()->root;
}