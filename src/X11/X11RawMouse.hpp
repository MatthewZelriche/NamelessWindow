#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include "NamelessWindow/RawMouse.hpp"
#include "X11GenericMouse.hpp"
#include "X11RawInputDevice.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE X11RawMouse : public X11GenericMouse, public X11RawInputDevice, public RawMouse {
   public:
   X11RawMouse(MouseDeviceInfo device);

   private:
   Event PackageNewRawButtonPressEvent(xcb_input_button_press_event_t *event);
   Event PackageNewRawButtonReleaseEvent(xcb_input_button_press_event_t *event);
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;
   const xcb_input_xi_event_mask_t m_inputEventMask {
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS |
                                  XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE |
                                  XCB_INPUT_XI_EVENT_MASK_RAW_MOTION)};
};

}  // namespace NLSWIN