#pragma once
#include <xcb/xcb.h>

#include <vector>

#include "EventListener.x11.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE Window::Impl : public EventListenerX11 {
   private:
   static xcb_connection_t *m_xServerConnection;
   const uint32_t m_eventMask = XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                                XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_STRUCTURE_NOTIFY;
   int m_preferredScreenNum {0};
   xcb_window_t m_x11WindowID {0};
   bool receivedTerminateSignal {false};
   WindowMode m_currentWindowMode {WindowMode::WINDOWED};
   std::vector<Keyboard> m_keyboards;
   int m_width = 0;
   int m_height = 0;

   [[nodiscard]] xcb_screen_t *GetScreenFromMonitor(Monitor monitor) const;
   void ToggleFullscreen() noexcept;

   public:
   Impl(WindowProperties properties, const Window &window);
   void SetFullscreen(bool borderless) noexcept;
   void SetWindowed() noexcept;
   void Close() noexcept;
   void ProcessGenericEvent(xcb_generic_event_t *event) override;
   void AddKeyboard(const Keyboard &keyboard);

   [[nodiscard]] inline xcb_connection_t *GetConnection() const noexcept { return m_xServerConnection; }
   [[nodiscard]] inline xcb_window_t GetWindowID() const noexcept { return m_x11WindowID; }
   [[nodiscard]] inline bool RequestedClose() const noexcept { return receivedTerminateSignal; }
   [[nodiscard]] inline WindowMode GetWindowMode() const noexcept { return m_currentWindowMode; }
};
}  // namespace NLSWIN
