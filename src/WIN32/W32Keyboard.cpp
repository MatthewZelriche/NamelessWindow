#include "W32Keyboard.hpp"

#include "NamelessWindow/Exceptions.hpp"
#include "Events/W32EventBus.hpp"
#include "W32Util.hpp"

using namespace NLSWIN;

std::shared_ptr<Keyboard> Keyboard::Create() {
   return Create(KeyboardDeviceInfo());
}

std::shared_ptr<Keyboard> Keyboard::Create(KeyboardDeviceInfo device) {
   std::shared_ptr<W32Keyboard> impl = std::make_shared<W32Keyboard>(device);
   W32EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

W32Keyboard::W32Keyboard(KeyboardDeviceInfo device) {
   if (device.platformSpecificIdentifier != 0) {
      deviceSpecifier = device.platformSpecificIdentifier;
   }
   RAWINPUTDEVICE rawDevice {0};
   rawDevice.usUsage = 6;
   rawDevice.usUsagePage = 1;
   rawDevice.dwFlags = 0;
   rawDevice.hwndTarget = NULL;

   if (!RegisterRawInputDevices(&rawDevice, 1, sizeof(rawDevice))) {
      throw PlatformInitializationException();
   }

   // Update the toggle status on first construction, in case we've constructed 
   // a keyboard while a key was already toggled. 
   UpdateToggleKeyStatus();
}

void W32Keyboard::UpdateToggleKeyStatus() {
   // TODO: Is GetKeyState performant?
   // We use GetKeyState because its an easy way to query if a key is toggled
   USHORT capsToggled = GetKeyState(VK_CAPITAL);
   USHORT scrollToggled = GetKeyState(VK_SCROLL);
   USHORT numToggled = GetKeyState(VK_NUMLOCK);
   // The result has its lower-order bit set to 1 if the key is toggled, so perform a logical AND.
   if (capsToggled & 1) {
      capsLockOn = true;
   } else {
      capsLockOn = false;
   }

   if (scrollToggled & 1) {
      scrollLockOn = true;
   } else {
      scrollLockOn = false;
   }

   if (numToggled & 1) {
      numLockOn = true;
   } else {
      numLockOn = false;
   }
}

void W32Keyboard::ProcessGenericEvent(MSG event) {
   // Always remember to convert the WParam to our special type
   // @see WParamWithWindowHandle
   WParamWithWindowHandle* wParam = reinterpret_cast<WParamWithWindowHandle*>(event.wParam);

   switch (event.message) {
      case WM_SETFOCUS: {
         keyboardFocusedWindow = (HWND)wParam->wParam;
         break;
      }
      case WM_INPUT: {
         if (GetSubscribedWindows().count(keyboardFocusedWindow)) {
            // WM_INPUT is a special case, we we transform the raw input on the event thread.
            // This requires a heap allocation and we must be careful to delete it at the end of this method.
            // See W32EventThreadDispatcher for details.
            RAWINPUT* inputStruct = reinterpret_cast<RAWINPUT*>(event.lParam);
            if (inputStruct->header.dwType == RIM_TYPEKEYBOARD) {
               // Only process if we are specifically interested in this device, or its a master device.
               // We additionally ignore inputs of 255/0xff, because windows sends fake input events with 
               // this value for certain keys.
               if ((((uint64_t)inputStruct->header.hDevice == deviceSpecifier) || deviceSpecifier == 0) &&
               inputStruct->data.keyboard.VKey != 0xff) {
                  PushEvent(ProcessKeyEvent(inputStruct->data.keyboard, keyboardFocusedWindow));
               }
            }
         }
         break;
      }
   }
}

Event W32Keyboard::ProcessKeyEvent(RAWKEYBOARD event, HWND window) {
   KeyEvent keyEvent;
   USHORT finalVKey = DeobfuscateWindowsVKey(event);
   //keyEvent.code.value = (KeyValue)finalVKey;
   if (m_translationTable.count(finalVKey)) {
      keyEvent.code.value = m_translationTable[finalVKey];
   } else {
      return KeyEvent();
   }
   // TODO: Performance? This is in a hot loop after all.
   keyEvent.keyName = magic_enum::enum_name(keyEvent.code.value);

   // Determine press type
   if (event.Flags & RI_KEY_BREAK) {
      m_InternalKeyState[keyEvent.code.value] = false;
      keyEvent.pressType = KeyPressType::RELEASED;
   } else {
      if (m_InternalKeyState[keyEvent.code.value] == true) {
         keyEvent.pressType = KeyPressType::REPEAT;
      } else {
         keyEvent.pressType = KeyPressType::PRESSED;
         m_InternalKeyState[keyEvent.code.value] = true;
      }
   }

   // Get Window ID
   auto win = GetSubscribedWindows().at(keyboardFocusedWindow);
   if (!win.expired()) {
      auto winPtr = win.lock();
      keyEvent.sourceWindow = winPtr->GetGenericID();
   }

   UpdateToggleKeyStatus();
   keyEvent.code.modifiers = ParseModifierState();

   return keyEvent;
}

KeyModifiers W32Keyboard::ParseModifierState() {
   KeyModifiers modifiers {false};
   if (m_InternalKeyState[VK_LCONTROL] || m_InternalKeyState[VK_RCONTROL]) {
      modifiers.ctrl = true;
   }
   if (m_InternalKeyState[VK_LWIN] || m_InternalKeyState[VK_RWIN]) {
      modifiers.super = true;
   }
   if (m_InternalKeyState[VK_LMENU] || m_InternalKeyState[VK_RMENU]) {
      modifiers.alt = true;
   }
   if (m_InternalKeyState[VK_LSHIFT] || m_InternalKeyState[VK_RSHIFT]) {
      modifiers.shift = true;
   }
   if (capsLockOn) {
      modifiers.capsLock = true;
   }
   if (scrollLockOn) {
      modifiers.scrollLock = true;
   } 
   if (numLockOn) {
      modifiers.numLock = true;
   }
   return modifiers;
}

USHORT W32Keyboard::DeobfuscateWindowsVKey(RAWKEYBOARD obfuscatedEvent) {
   // Adapted from the following:
   // https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
   // Copyright(c) 2012-2017 Stefan Reinalter
   // See NOTICE.md for full license.
   USHORT vkey = obfuscatedEvent.VKey;

   switch (vkey) {
      case VK_SHIFT:
         vkey = MapVirtualKey(obfuscatedEvent.MakeCode, MAPVK_VSC_TO_VK_EX);
         break;
      case VK_CONTROL:
         if (obfuscatedEvent.Flags & RI_KEY_E0) {
            vkey = VK_RCONTROL;
         } else {
            vkey = VK_LCONTROL;
         }
         break;
      case VK_MENU:
         if (obfuscatedEvent.Flags & RI_KEY_E0) {
            vkey = VK_RMENU;
         } else {
            vkey = VK_LMENU;
         }
         break;
   }
   return vkey;
}

std::vector<KeyboardDeviceInfo> Keyboard::EnumerateKeyboards() noexcept {
   return std::get<std::vector<KeyboardDeviceInfo>>(GetDeviceList(RIM_TYPEKEYBOARD));
}