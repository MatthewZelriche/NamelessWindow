#include "X11InputDevice.hpp"

#include "X11Util.hpp"
#include "X11Window.hpp"

using namespace NLSWIN;

void X11InputDevice::SubscribeToWindow(const std::weak_ptr<Window> x11Window) {
   if (!x11Window.expired()) {
      std::shared_ptr<X11Window> windowSharedPtr = std::static_pointer_cast<X11Window>(x11Window.lock());
      m_subscribedWindows.insert(
         std::make_pair(windowSharedPtr->GetX11ID(), std::static_pointer_cast<X11Window>(windowSharedPtr)));

      UTIL::XI2EventMask mask;
      mask.header.deviceid = m_deviceID;
      mask.header.mask_len = sizeof(mask.mask) / sizeof(uint32_t);
      mask.mask = m_subscribedXInputMask;
      xcb_input_xi_select_events_checked(XConnection::GetConnection(), windowSharedPtr->GetX11ID(), 1,
                                         &mask.header);
      xcb_flush(XConnection::GetConnection());
   }
}

void X11InputDevice::UnsubscribeFromWindow(const std::weak_ptr<Window> x11Window) {
   if (!x11Window.expired()) {
      std::shared_ptr<X11Window> windowSharedPtr = std::static_pointer_cast<X11Window>(x11Window.lock());
      m_subscribedWindows.erase(windowSharedPtr->GetX11ID());

      UTIL::XI2EventMask mask;
      mask.header.deviceid = m_deviceID;
      mask.header.mask_len = 0;  // Length of zero clears the mask on the X server,
      xcb_input_xi_select_events_checked(XConnection::GetConnection(), windowSharedPtr->GetX11ID(), 1,
                                         &mask.header);
      xcb_flush(XConnection::GetConnection());
   }
}