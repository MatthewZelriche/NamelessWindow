/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup X11 Linux X11 API
 * @brief Platform-specific X11 implementation of the API
 */
#pragma once

#include <xcb/xcb.h>

#include "NamelessWindow/Window.hpp"
#include "X11EventListener.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE X11Window : public Window, public X11EventListener {
   public:
   void DisableUserResizing() override;
   void EnableUserResizing() override;
   void Show() override;
   void Hide() override;

   /**
    * @brief Construct a new X11Window object
    * @throws PlatformInitializationException
    * @param properties The properties that should define this window.
    */
   X11Window(WindowProperties properties);
   /**
    * @brief Gets the default X screen.
    *
    * @return The default X screen.
    */
   static xcb_screen_t *GetDefaultScreen();

   private:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;
   xcb_screen_t *m_defaultScreen {nullptr};
   xcb_window_t m_rootWindow {0};
   xcb_window_t m_x11WindowID {0};
   unsigned int m_preferredBorderWidth {0};
   unsigned int m_preferredXCoord {0};
   unsigned int m_preferredYCoord {0};
   unsigned int m_preferredWidth {0};
   unsigned int m_preferredHeight {0};
   unsigned int m_width {0};
   unsigned int m_height {0};
   bool m_isMapped {false};
   const xcb_event_mask_t m_eventMask =
      (xcb_event_mask_t)(XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_FOCUS_CHANGE |
                         XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_ENTER_WINDOW |
                         XCB_EVENT_MASK_VISIBILITY_CHANGE);
};
}  // namespace NLSWIN