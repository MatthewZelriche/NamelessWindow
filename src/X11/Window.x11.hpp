#pragma once
#include <xcb/xcb.h>

#include <vector>

#include "EventListener.x11.hpp"
#include "MasterPointer.x11.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/Window.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE WindowX11 : public Window, public EventListenerX11 {
   private:
   xcb_connection_t *m_xServerConnection;
   const uint32_t m_eventMask = XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                                XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                                XCB_EVENT_MASK_ENTER_WINDOW;
   int m_preferredScreenNum {0};
   xcb_window_t m_x11WindowID {0};
   xcb_window_t m_rootWindow {0};
   bool receivedTerminateSignal {false};
   WindowMode m_currentWindowMode {WindowMode::WINDOWED};
   std::vector<Keyboard> m_keyboards;
   int m_width = 0;
   int m_height = 0;
   bool m_isUserResizable {true};

   static std::shared_ptr<MasterPointerX11> m_masterPointer;

   inline static uint32_t NewGenericWindowID() {
      static uint32_t newID;
      return newID++;
   }
   WindowID m_genericWindowID = NewGenericWindowID();

   void RegisterWindowForMasterPointerEvents();

   [[nodiscard]] xcb_screen_t *GetScreenFromMonitor(Monitor monitor) const;
   void ProcessGenericEvent(xcb_generic_event_t *event) override;
   void ToggleFullscreen() noexcept;

   public:
   WindowX11(WindowProperties properties);
   void SetFullscreen(bool borderless) noexcept;
   void SetWindowed() noexcept;
   void Close() noexcept;
   Pointer &GetMasterPointer() override;
   void RepositionWindow(uint32_t newX, uint32_t newY) override;
   void SetUserResizable(bool isResizable) override;
   void Resize(uint32_t width, uint32_t height) override;

   [[nodiscard]] inline xcb_connection_t *GetConnection() const noexcept { return m_xServerConnection; }
   [[nodiscard]] inline xcb_window_t GetX11WindowID() const noexcept { return m_x11WindowID; }
   [[nodiscard]] inline xcb_window_t GetX11RootWindowID() const noexcept { return m_rootWindow; }
   [[nodiscard]] inline bool RequestedClose() const noexcept { return receivedTerminateSignal; }
   [[nodiscard]] inline WindowMode GetWindowMode() const noexcept { return m_currentWindowMode; }
   [[nodiscard]] inline WindowID GetWindowID() const noexcept { return m_genericWindowID; }
   [[nodiscard]] inline unsigned int GetWindowWidth() const noexcept override { return m_width; }
   [[nodiscard]] inline unsigned int GetWindowHeight() const noexcept override { return m_height; }
};
}  // namespace NLSWIN
