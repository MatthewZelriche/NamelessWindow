#include "W32Keyboard.hpp"

#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <devguid.h>
#include <hidsdi.h>

#include <string>
// clang-format off
#include <initguid.h>
#include <Devpkey.h>
// clang-format on

#include "NamelessWindow/Exceptions.hpp"
#include "Events/W32EventBus.hpp"

using namespace NLSWIN;
/**
 * @brief Returns a unique string identifying a particular device instance.
 *
 * @param interfaceName The name for a PnP device interface.
 * @return std::wstring The unique Instance ID string.
 */
static std::wstring GetDeviceInstanceID(const std::wstring &interfaceName);

/**
 * @brief Get a less-preferred, fallback friendly name (corresponding to the device's generic description in
 * the device manager) to identify a particular device instance.
 *
 * This function is used only if an attempt to get an HID Product String fails. The HID Product
 * string is preferred as it is more descriptive for USB devices. This function is mostly useful
 * for enumerating non-USB keyboards.
 *
 * @param InstanceID The unique string identifying a particular device instance.
 * @return std::wstring A friendly name, or an empty string if none could be found.
 */
static std::wstring GetFallbackStringName(const std::wstring &InstanceID);

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
            // My understanding is that WM_INPUT data structs never exceed 80 bytes.
            // We statically allocate a buffer of memory to avoid frequent new/delete calls.
            static unsigned int dataSize = 80;
            static std::vector<uint8_t> inputStructBuf(dataSize);
            GetRawInputData((HRAWINPUT)event.lParam, RID_INPUT, inputStructBuf.data(),
                  &dataSize, sizeof(RAWINPUTHEADER));
            RAWINPUT *inputStruct = reinterpret_cast<RAWINPUT*>(inputStructBuf.data());
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
   std::vector<KeyboardDeviceInfo> infos;
   // Get a list of raw devices.
   RAWINPUTDEVICELIST *list = nullptr;
   unsigned int numDevices = 0;
   GetRawInputDeviceList(nullptr, &numDevices, sizeof(RAWINPUTDEVICELIST));
   list = new RAWINPUTDEVICELIST[numDevices];
   if (GetRawInputDeviceList(list, &numDevices, sizeof(RAWINPUTDEVICELIST)) == -1) {
      return {};
   }
   for (int i = 0; i < numDevices; i++) {
      // Check if device is a keyboard.
      if (list[i].dwType == RIM_TYPEKEYBOARD) {
         // Get interface name
         unsigned int interfaceNameSize = 0;
         GetRawInputDeviceInfoW(list[i].hDevice, RIDI_DEVICENAME, nullptr, &interfaceNameSize);
         std::wstring interfaceName;
         interfaceName.resize(interfaceNameSize + 1);
         GetRawInputDeviceInfoW(list[i].hDevice, RIDI_DEVICENAME, (LPVOID)interfaceName.c_str(),
                                &interfaceNameSize);
         GetLastError();
         HANDLE fileHandle =
            CreateFileW(interfaceName.c_str(), 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
         if (fileHandle == INVALID_HANDLE_VALUE) {
            continue;
         }
         // Get a user-friendly name for the keyboard.
         std::wstring productName;
         productName.resize(126);
         if (!HidD_GetProductString(fileHandle, (PVOID)productName.c_str(), 126 * sizeof(wchar_t))) {
            // We've failed to get a product string and need to fall back on a secondary method of
            // getting a name. In practice, this is currently useful for PS/2 keyboards.
            std::wstring instanceID = GetDeviceInstanceID(interfaceName);
            productName = GetFallbackStringName(instanceID);
            if (productName.size() == 0) {
               // We couldn't even find a fallback, just give up on this device.
               continue;
            }
         }
         // Convert from wstring to cross-platform string
         // TODO: Should we use wstring on all platforms?
         std::string finalDeviceName(productName.begin(), productName.end());
         infos.emplace_back(KeyboardDeviceInfo {finalDeviceName, (uint64_t)list[i].hDevice});
      }
   }
   return infos;
}

static std::wstring GetDeviceInstanceID(const std::wstring &interfaceName) {
   DEVPROPTYPE propertyType;
   ULONG bufSize = 0;
   CM_Get_Device_Interface_PropertyW(interfaceName.c_str(), &DEVPKEY_Device_InstanceId, &propertyType,
                                     nullptr, &bufSize, 0);
   std::wstring ID;
   ID.resize(bufSize + 1);
   CM_Get_Device_Interface_PropertyW(interfaceName.c_str(), &DEVPKEY_Device_InstanceId, &propertyType,
                                     (PBYTE)ID.c_str(), &bufSize, 0);
   return ID;
}

static std::wstring GetFallbackStringName(const std::wstring &instanceID) {
   HDEVINFO infoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_KEYBOARD, nullptr, nullptr, DIGCF_PRESENT);
   int memberIndex = 0;
   while (true) {
      SP_DEVINFO_DATA data = {0};
      data.cbSize = sizeof(SP_DEVINFO_DATA);
      SetupDiEnumDeviceInfo(infoSet, memberIndex++, &data);
      int result = GetLastError();
      if (result == ERROR_NO_MORE_ITEMS) {
         break;
      } else if (result != 0) {
         continue;
      }
      std::wstring currentInfoID;
      currentInfoID.resize(MAX_DEVICE_ID_LEN);
      CM_Get_Device_IDW(data.DevInst, (PWSTR)currentInfoID.c_str(), MAX_DEVICE_ID_LEN, 0);
      if (_wcsicmp(instanceID.c_str(), currentInfoID.c_str()) == 0) {
         unsigned long descSize = 0;
         SetupDiGetDeviceRegistryPropertyW(infoSet, &data, SPDRP_DEVICEDESC, nullptr, nullptr, 0, &descSize);
         std::wstring fallbackName;
         fallbackName.resize(descSize + 1);
         SetupDiGetDeviceRegistryPropertyW(infoSet, &data, SPDRP_DEVICEDESC, nullptr,
                                           (PBYTE)fallbackName.data(), descSize + 1, nullptr);
         return fallbackName;
      }
   }
   // Couldn't find it...Return empty string.
   return std::wstring();
}