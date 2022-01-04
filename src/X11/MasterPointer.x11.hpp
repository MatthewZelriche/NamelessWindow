#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include "NamelessWindow/Pointer.hpp"
#include "PointerDevice.x11.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE MasterPointerX11 : public PointerDeviceX11, public MasterPointer {
   private:
   static bool m_instantiated;
   bool m_clientRequestedHiddenCursor {false};
   bool m_cursorHidden {false};
   static xcb_input_device_id_t GetMasterPointerDeviceID();
   xcb_input_device_id_t m_corePointerID {0};
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;

   public:
   MasterPointerX11();
   void HideCursor() noexcept override;
   void SetCursorInvisible();
   void ShowCursor() noexcept override;
   void SetCursorVisible();
   void OnFocusOut(xcb_focus_out_event_t *event);
   void BindToWindow(const Window *const window) noexcept override;
   void UnbindFromWindow() noexcept override;

   [[nodiscard]] inline bool ClientRequestedHiddenCursor() { return m_clientRequestedHiddenCursor; };
   [[nodiscard]] inline xcb_input_device_id_t GetMasterPointerID() const noexcept { return m_corePointerID; }

   ~MasterPointerX11() = default;
};
}  // namespace NLSWIN