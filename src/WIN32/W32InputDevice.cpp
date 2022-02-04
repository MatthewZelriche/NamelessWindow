#include "W32InputDevice.hpp"

using namespace NLSWIN;

void W32InputDevice::SubscribeToWindow(const std::weak_ptr<Window> window) {
   if (!window.expired()) {
      std::shared_ptr<W32Window> windowSharedPtr = std::static_pointer_cast<W32Window>(window.lock());
      m_subscribedWindows.insert(std::make_pair(windowSharedPtr->GetWin32Handle(),
                                                std::static_pointer_cast<W32Window>(windowSharedPtr)));
   }
}

void W32InputDevice::UnsubscribeFromWindow(const std::weak_ptr<Window> window) {
   if (!window.expired()) {
      std::shared_ptr<W32Window> windowSharedPtr = std::static_pointer_cast<W32Window>(window.lock());
      m_subscribedWindows.erase(windowSharedPtr->GetWin32Handle());
   }
}