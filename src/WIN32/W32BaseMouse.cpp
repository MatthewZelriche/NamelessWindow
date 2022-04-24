#include "W32BaseMouse.hpp"

#include <HidUsage.h>
#include "NamelessWindow/Exceptions.hpp"

using namespace NLSWIN;

bool W32BaseMouse::s_firstInit {true};
std::shared_ptr<W32Window> W32BaseMouse::s_rawInputHandle {nullptr};

W32BaseMouse::W32BaseMouse() {
   // Set up a dummy window once that will be used for all raw mouse instances.
   if (s_firstInit) {
      std::shared_ptr<Window> test = Window::Create();
      s_rawInputHandle = std::static_pointer_cast<W32Window>(Window::Create());
      s_firstInit = false;
   }

   // Register our raw mouse device.
   RAWINPUTDEVICE rawDevice {0};
   rawDevice.usUsage = HID_USAGE_GENERIC_MOUSE;
   rawDevice.usUsagePage = HID_USAGE_PAGE_GENERIC;
   // Use RIDEV_INPUTSINK to get raw mouse events from our dummy window even when
   // its not in focus.
   rawDevice.dwFlags = RIDEV_INPUTSINK;
   rawDevice.hwndTarget = s_rawInputHandle->GetWin32Handle();

   if (!RegisterRawInputDevices(&rawDevice, 1, sizeof(rawDevice))) {
      throw PlatformInitializationException();
   }
}

Event W32BaseMouse::PackageRawDeltaEvent(RAWMOUSE mouse) {
      RawMouseDeltaMovementEvent rawMouseEvent {};
      rawMouseEvent.deltaX = mouse.lLastX;
      rawMouseEvent.deltaY = mouse.lLastY;
      return rawMouseEvent;
}