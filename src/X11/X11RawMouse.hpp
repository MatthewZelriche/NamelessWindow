#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include "NamelessWindow/RawMouse.hpp"
#include "X11GenericMouse.hpp"

namespace NLSWIN {

class NLSWIN_API_PRIVATE X11RawMouse : public X11GenericMouse, virtual public RawMouse {
   public:
   X11RawMouse(MouseDeviceInfo device);

   protected:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;

   private:
   void PushNewRawButtonPressEvent(xcb_input_raw_button_press_event_t *event);
   void PushNewRawButtonReleaseEvent(xcb_input_raw_button_press_event_t *event);
   const xcb_input_xi_event_mask_t m_inputEventMask {
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_RAW_BUTTON_PRESS |
                                  XCB_INPUT_XI_EVENT_MASK_RAW_BUTTON_RELEASE |
                                  XCB_INPUT_XI_EVENT_MASK_RAW_MOTION)};
};

}  // namespace NLSWIN