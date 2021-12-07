#pragma once
#include <xcb/xcb.h>

#include <vector>

#include "EventListener.x11.hpp"
#include "NamelessWindow/Event.hpp"
#include "NamelessWindow/Window.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE Window::WindowImpl : public EventListenerX11 {
   private:
   static xcb_connection_t *m_xServerConnection;
   int m_preferredScreenNum {0};
   xcb_window_t m_x11WindowID {0};

   xcb_screen_t *GetScreenFromMonitor(Monitor monitor);
   void ToggleFullscreen();
   WindowMode m_currentWindowMode;
   bool receivedTerminateSignal {false};

   public:
   WindowImpl(WindowProperties properties, const Window &window);
   void SetFullscreen(bool borderless);
   void SetWindowed();
   void Close();

   inline xcb_connection_t *GetConnection() const { return m_xServerConnection; }
   inline xcb_window_t GetWindowID() const { return m_x11WindowID; }

   inline bool RequestedClose() const { return receivedTerminateSignal; }
   inline WindowMode GetWindowMode() const { return m_currentWindowMode; }

   // We override EventRecieved because we need a way to process window close events internally.
   void EventRecieved(Event event) override;
};
}  // namespace NLSWIN
