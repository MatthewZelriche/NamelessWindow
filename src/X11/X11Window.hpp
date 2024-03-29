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

#include <GL/glx.h>
#include <xcb/xcb.h>

#include <unordered_map>
#include <array>

#include "NamelessWindow/Window.hpp"
#include "X11EventListener.hpp"

namespace NLSWIN {

struct DecorationSizes {
   long left {0};
   long right {0};
   long top {0};
   long bottom {0};
};

/*!
 * @brief An instance of an X window.
 * @ingroup X11
 */
class NLSWIN_API_PRIVATE X11Window : public NLSWIN::Window, public X11EventListener {
   public:
   void Show() override;
   void Hide() override;
   void SetFullscreen() override;
   void SetWindowed() noexcept override;
   void Reposition(uint32_t newX, uint32_t newY) noexcept override;
   void Resize(uint32_t width, uint32_t height) noexcept override;
   void Focus() noexcept override;
   void EnableBorderless() noexcept override;
   void DisableBorderless() noexcept override;
   void Minimize(bool restoreVideoMode = false) override;
   [[nodiscard]] bool IsBorderless() const noexcept override { return m_isBorderless; }
   inline bool RequestedClose() const noexcept override { return m_shouldClose; }
   inline WindowMode GetWindowMode() const noexcept override { return m_windowMode; }
   [[nodiscard]] Point GetWindowPos() const noexcept override {
      return Point {m_windowGeometry.x, m_windowGeometry.y};
   }
   unsigned int GetWindowWidth() const noexcept override { return m_windowGeometry.width; }
   unsigned int GetWindowHeight() const noexcept override { return m_windowGeometry.height; }

   X11Window(WindowProperties properties);
   ~X11Window();
   void ToggleFullscreen() noexcept;
   void SetVideoMode(uint32_t width, uint32_t height);

   [[nodiscard]] inline xcb_window_t GetX11ID() const noexcept { return m_x11WindowID; }
   [[nodiscard]] inline Rect GetWindowGeometry() const noexcept { return m_windowGeometry; }
   [[nodiscard]] inline xcb_window_t GetRootWindow() const noexcept { return m_rootWindow; }
   [[nodiscard]] inline xcb_visualid_t GetSelectedVisualID() const noexcept { return m_selectedVisual; }
   [[nodiscard]] inline const std::array<int, 21> &GetVisualAttributes() const noexcept {
      return m_visualAttributesList;
   }

   static std::unordered_map<xcb_window_t, WindowID> m_handleMap;
   [[nodiscard]] static inline bool IsUserWindow(xcb_window_t handle) { return m_handleMap.count(handle); }
   [[nodiscard]] static inline WindowID IDFromHWND(xcb_window_t handle) { return m_handleMap.at(handle); }

   private:
   // Used only on window creation.
   WindowMode m_firstMapCachedMode {WindowMode::WINDOWED};
   xcb_visualid_t SelectAppropriateVisualIDForGL(std::optional<GLConfiguration> config);
   void SetVisualAttributeProperty(int property, int value);
   int m_visualDepth {0};
   int m_selectedVisual {0};
   Rect m_windowGeometry;
   void ProcessGenericEvent(xcb_generic_event_t *event) override;
   Rect GetNewGeometry();
   WindowMode m_windowMode {WindowMode::WINDOWED};
   xcb_screen_t *m_defaultScreen {nullptr};
   xcb_window_t m_rootWindow {0};
   xcb_window_t m_x11WindowID {0};
   unsigned int m_preferredBorderWidth {0};
   unsigned int m_preferredXCoord {0};
   unsigned int m_preferredYCoord {0};
   unsigned int m_preferredWidth {0};
   unsigned int m_preferredHeight {0};
   bool m_isMapped {false};
   bool m_shouldClose {false};
   bool m_isBorderless {false};
   DecorationSizes m_decoDimensions;

   std::array<int, 21> m_visualAttributesList = {GLX_X_RENDERABLE,
                                                 True,
                                                 GLX_RENDER_TYPE,
                                                 GLX_RGBA_BIT,
                                                 GLX_DRAWABLE_TYPE,
                                                 GLX_WINDOW_BIT,
                                                 GLX_X_VISUAL_TYPE,
                                                 GLX_TRUE_COLOR,
                                                 GLX_DOUBLEBUFFER,
                                                 True,
                                                 GLX_DEPTH_SIZE,
                                                 24,
                                                 GLX_RED_SIZE,
                                                 8,
                                                 GLX_GREEN_SIZE,
                                                 8,
                                                 GLX_BLUE_SIZE,
                                                 8,
                                                 GLX_ALPHA_SIZE,
                                                 8,
                                                 None};
   const xcb_event_mask_t m_eventMask =
      (xcb_event_mask_t)(XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_FOCUS_CHANGE |
                         XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_ENTER_WINDOW |
                         XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_VISIBILITY_CHANGE |
                         XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                         XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_PROPERTY_CHANGE);
};
}  // namespace NLSWIN