#include "W32RawMouse.hpp"

#include <hidusage.h>

#include "Events/W32EventBus.hpp"
#include "Events/W32EventThreadDispatcher.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "W32DllMain.hpp"
#include "W32Util.hpp"

using namespace NLSWIN;

bool W32RawMouse::s_firstInit {true};
std::shared_ptr<W32Window> W32RawMouse::s_rawInputHandle {nullptr};

std::vector<MouseDeviceInfo> RawMouse::EnumeratePointers() noexcept {
   return std::get<std::vector<MouseDeviceInfo>>(GetDeviceList(RIM_TYPEMOUSE));
}

std::shared_ptr<RawMouse> RawMouse::Create(MouseDeviceInfo device) {
   std::shared_ptr<W32RawMouse> impl = std::make_shared<W32RawMouse>(device);
   W32EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

W32RawMouse::W32RawMouse(MouseDeviceInfo device) {
   // Set up a dummy window once that will be used for all raw mouse instances.
   if (s_firstInit) {
      std::shared_ptr<Window> test = Window::Create();
      s_rawInputHandle = std::static_pointer_cast<W32Window>(Window::Create());
      s_firstInit = false;
   }

   m_deviceSpecifier = device.platformSpecificIdentifier;

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

void W32RawMouse::ProcessGenericEvent(MSG event) {
   // Always remember to convert the WParam to our special type
   // @see WParamWithWindowHandle
   WParamWithWindowHandle* wParam = reinterpret_cast<WParamWithWindowHandle*>(event.wParam);
   switch (event.message) {
      case WM_INPUT: {
         // My understanding is that WM_INPUT data structs never exceed 80 bytes.
         // We statically allocate a buffer of memory to avoid frequent new/delete calls.
         static unsigned int dataSize = 80;
         static std::vector<uint8_t> inputStructBuf(dataSize);
         GetRawInputData((HRAWINPUT)event.lParam, RID_INPUT, inputStructBuf.data(), &dataSize,
                         sizeof(RAWINPUTHEADER));
         RAWINPUT* inputStruct = reinterpret_cast<RAWINPUT*>(inputStructBuf.data());
         if (inputStruct->header.dwType == RIM_TYPEMOUSE) {
            if (inputStruct->data.mouse.lLastX != 0 || inputStruct->data.mouse.lLastY) {
               // We got some new raw mouse input.
               // Is this the hardware device we are listening for?
               if ((uint64_t)inputStruct->header.hDevice == m_deviceSpecifier) {
                  RawMouseDeltaMovementEvent rawMouseEvent {};
                  rawMouseEvent.deltaX = inputStruct->data.mouse.lLastX;
                  rawMouseEvent.deltaY = inputStruct->data.mouse.lLastY;
                  PushEvent(rawMouseEvent);
               }
            }
         }
         break;
      }
   }
}