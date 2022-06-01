#pragma once
#include <windows.h>

#include <unordered_map>

#include "Events/W32EventListener.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "NamelessWindow/SubscribableInputDevice.hpp"
#include "W32Window.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE W32InputDevice :
   virtual public SubscribableInputDevice,
   virtual public W32EventListener {
   public:
   void SubscribeToWindow(const std::weak_ptr<Window> window) override;
   void UnsubscribeFromWindow(const std::weak_ptr<Window> window) override;

   protected:
   const std::unordered_map<HWND, std::weak_ptr<W32Window>> &GetSubscribedWindows() const noexcept {
      return m_subscribedWindows;
   }

   private:
   std::unordered_map<HWND, std::weak_ptr<W32Window>> m_subscribedWindows;
};
}  // namespace NLSWIN