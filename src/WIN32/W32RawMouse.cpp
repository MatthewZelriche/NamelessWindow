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
   if (event.message == WM_INPUT) {
       // WM_INPUT is a special case, we we transform the raw input on the event thread. 
       // This requires a heap allocation and we must be careful to delete it at the end of this method. 
       // See W32EventThreadDispatcher for details.
      RAWINPUT* inputStruct = reinterpret_cast<RAWINPUT*>(event.lParam);
      if (inputStruct->header.dwType == RIM_TYPEMOUSE) {
         // We got some new raw mouse input.
         // Is this the hardware device we are listening for?
         if ((uint64_t)inputStruct->header.hDevice == m_deviceSpecifier) {
            if (inputStruct->data.mouse.lLastX != 0 || inputStruct->data.mouse.lLastY) {
               RawMouseDeltaMovementEvent rawMouseEvent {};
               rawMouseEvent.deltaX = inputStruct->data.mouse.lLastX;
               rawMouseEvent.deltaY = inputStruct->data.mouse.lLastY;
               PushEvent(rawMouseEvent);
            }
            // Has the button state changed?
            if (inputStruct->data.mouse.usButtonFlags) {
               if (inputStruct->data.mouse.usButtonFlags == RI_MOUSE_WHEEL) {
                  RawMouseScrollEvent scrollEvent;
                  short signedDir = inputStruct->data.mouse.usButtonData;
                  if (signedDir > 0) {
                     scrollEvent.scrollType = ScrollType::UP;
                  } else {
                     scrollEvent.scrollType = ScrollType::DOWN;
                  }
                  PushEvent(scrollEvent);
               } else {
                  // Handle regular buttons.
                  PushEvent(PackageButtonEvent(inputStruct->data.mouse));
               }
            }
         }
      }
      delete (void*)(event.lParam);
   }
}

ButtonValue W32RawMouse::TranslateButton(unsigned int win32ButtonID) {
   if (!m_buttonTranslationTable.count(win32ButtonID)) {
      return ButtonValue::NULLCLICK;
   }
   return m_buttonTranslationTable[win32ButtonID];
}

Event W32RawMouse::PackageButtonEvent(RAWMOUSE event) {
   RawMouseButtonEvent buttonEvent;
   switch (event.usButtonFlags) {
      case RI_MOUSE_LEFT_BUTTON_DOWN:
      case RI_MOUSE_RIGHT_BUTTON_DOWN:
      case RI_MOUSE_MIDDLE_BUTTON_DOWN:
      case RI_MOUSE_BUTTON_4_DOWN:
      case RI_MOUSE_BUTTON_5_DOWN: {
         buttonEvent.type = ButtonPressType::PRESSED;
         break;
      }
      case RI_MOUSE_LEFT_BUTTON_UP:
      case RI_MOUSE_RIGHT_BUTTON_UP:
      case RI_MOUSE_MIDDLE_BUTTON_UP:
      case RI_MOUSE_BUTTON_4_UP:
      case RI_MOUSE_BUTTON_5_UP: {
         buttonEvent.type = ButtonPressType::RELEASED;
         break;
      }
      default: {
         buttonEvent.type = ButtonPressType::UNKNOWN;
      }
   }
   buttonEvent.button = TranslateButton(event.usButtonFlags);
   int buttonIndex = (int)buttonEvent.button;
   if (!m_rawButtonState[buttonIndex] && buttonEvent.type == ButtonPressType::PRESSED) {
      m_rawButtonState[buttonIndex] = true;
      return buttonEvent;
   } else if (m_rawButtonState[buttonIndex] && buttonEvent.type == ButtonPressType::RELEASED) {
      m_rawButtonState[buttonIndex] = false;
      return buttonEvent;
   } else {
      return std::monostate();
   }
}