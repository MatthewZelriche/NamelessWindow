#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <unordered_map>

#include "EventListener.x11.hpp"
#include "NamelessWindow/NLSAPI.h"

namespace NLSWIN {
class Window;
struct NLSWIN_API_PRIVATE XI2EventMask {
   xcb_input_event_mask_t head;
   xcb_input_xi_event_mask_t mask;
};

class NLSWIN_API_PRIVATE InputDeviceX11 : public EventListenerX11 {
   private:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;

   protected:
   std::unordered_map<xcb_window_t, WindowID> m_SubscribedWindows;
   xcb_input_device_id_t m_deviceID;
   xcb_input_xi_event_mask_t m_subscribedMasks;
   xcb_connection_t *m_connection;

   virtual void ProcessXInputEvent(xcb_ge_generic_event_t *event) = 0;

   public:
   [[nodiscard]] xcb_input_device_id_t GetDeviceID() { return m_deviceID; }
   void SubscribeToWindow(xcb_window_t x11Handle, WindowID windowID);
};
}  // namespace NLSWIN