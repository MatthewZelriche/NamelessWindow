#pragma once
#include <xcb/xcb.h>

#include <vector>

#include "EventListener.x11.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Window.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE Window::Impl : public EventListenerX11 {
   private:
   static xcb_connection_t *m_xServerConnection;
   const uint32_t m_eventMask =
      XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_FOCUS_CHANGE;
   int m_preferredScreenNum {0};
   xcb_window_t m_x11WindowID {0};
   bool receivedTerminateSignal {false};
   WindowMode m_currentWindowMode;

   [[nodiscard]] xcb_screen_t *GetScreenFromMonitor(Monitor monitor) const;
   void ToggleFullscreen() noexcept;

   public:
   Impl(WindowProperties properties, const Window &window);
   void SetFullscreen(bool borderless) noexcept;
   void SetWindowed() noexcept;
   void Close() noexcept;
   // We override EventRecieved because we need a way to process window close events internally.
   void EventRecieved(Event event) override;

   [[nodiscard]] inline xcb_connection_t *GetConnection() const noexcept { return m_xServerConnection; }
   [[nodiscard]] inline xcb_window_t GetWindowID() const noexcept { return m_x11WindowID; }
   [[nodiscard]] inline bool RequestedClose() const noexcept { return receivedTerminateSignal; }
   [[nodiscard]] inline WindowMode GetWindowMode() const noexcept { return m_currentWindowMode; }
};
}  // namespace NLSWIN
