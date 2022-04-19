#include "W32Util.hpp"

// clang-format off
#include <windows.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <devguid.h>
#include <hidsdi.h>
#include <string>
#include <initguid.h>
#include <Devpkey.h>
// clang-format on

using namespace NLSWIN;

std::wstring NLSWIN::ConvertToWString(const char* originalString) {
   std::wstring test;
   int requiredSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, originalString, -1, test.data(), 0);
   test.resize(requiredSize);
   MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, originalString, -1, test.data(), requiredSize);
   return std::move(test);
}

std::wstring NLSWIN::ConvertToWString(std::string originalString) {
   return ConvertToWString(originalString.c_str());
}

/**
 * @brief Returns a unique string identifying a particular device instance.
 *
 * @param interfaceName The name for a PnP device interface.
 * @return std::wstring The unique Instance ID string.
 */
static std::wstring GetDeviceInstanceID(const std::wstring& interfaceName) {
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
static std::wstring GetFallbackStringName(const std::wstring& instanceID) {
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

std::variant<std::vector<KeyboardDeviceInfo>, std::vector<MouseDeviceInfo>> NLSWIN::GetDeviceList(
   int deviceType) {
   // Currently only mouse and kb supported.
   std::variant<std::vector<KeyboardDeviceInfo>, std::vector<MouseDeviceInfo>> infos;
   switch (deviceType) {
      case RIM_TYPEKEYBOARD:
         infos = std::vector<KeyboardDeviceInfo>();
         break;
      case RIM_TYPEMOUSE:
         infos = std::vector<MouseDeviceInfo>();
         break;
   }

   // Get a list of raw devices.
   RAWINPUTDEVICELIST* list = nullptr;
   unsigned int numDevices = 0;
   GetRawInputDeviceList(nullptr, &numDevices, sizeof(RAWINPUTDEVICELIST));
   list = new RAWINPUTDEVICELIST[numDevices];
   if (GetRawInputDeviceList(list, &numDevices, sizeof(RAWINPUTDEVICELIST)) == -1) {
      return {};
   }

   for (int i = 0; i < numDevices; i++) {
      if (list[i].dwType == deviceType) {
         // Get interface name
         unsigned int interfaceNameSize = 0;
         GetRawInputDeviceInfoW(list[i].hDevice, RIDI_DEVICENAME, nullptr, &interfaceNameSize);
         std::wstring interfaceName;
         interfaceName.resize(interfaceNameSize + 1);
         GetRawInputDeviceInfoW(list[i].hDevice, RIDI_DEVICENAME, (LPVOID)interfaceName.c_str(),
                                &interfaceNameSize);
         HANDLE fileHandle =
            CreateFileW(interfaceName.c_str(), 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
         if (fileHandle == INVALID_HANDLE_VALUE) {
            continue;
         }
         // Get a user-friendly name for the device.
         std::wstring productName;
         productName.resize(126);
         if (!HidD_GetProductString(fileHandle, (PVOID)productName.c_str(), 126 * sizeof(wchar_t))) {
            // We've failed to get a product string and need to fall back on a secondary method of
            // getting a name. In practice, this is currently useful for PS/2 keyboards.
            if (deviceType == RIM_TYPEKEYBOARD) {
               std::wstring instanceID = GetDeviceInstanceID(interfaceName);
               productName = GetFallbackStringName(instanceID);
               if (productName.size() == 0) {
                  // We couldn't even find a fallback, just give up on this device.
                  continue;
               }
            }
         }
         // Get final device name.
         std::string finalDeviceName(productName.begin(), productName.end());

         if (auto vector = std::get_if<std::vector<KeyboardDeviceInfo>>(&infos)) {
            vector->emplace_back(KeyboardDeviceInfo {finalDeviceName, (uint64_t)list[i].hDevice});
         } else if (auto vector = std::get_if<std::vector<MouseDeviceInfo>>(&infos)) {
            vector->emplace_back(MouseDeviceInfo {finalDeviceName, (uint64_t)list[i].hDevice});
         }
      }
   }
   return infos;
}