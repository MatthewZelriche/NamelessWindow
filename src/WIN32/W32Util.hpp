#pragma once

#include <string>
#include <variant>
#include <vector>

#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/RawMouse.hpp"

namespace NLSWIN {

/**
 * @brief Converts a null-terminated string to a Win32 UTF-16 wide string, for use with Win32 API.
 *
 * @param originalString The string to convert.
 * @return std::wstring The converted string.
 */
std::wstring ConvertToWString(const char* originalString);

/**
 * @brief Converts a null-terminated string to a Win32 UTF-16 wide string, for use with Win32 API.
 *
 * @param originalString The string to convert.
 * @return std::wstring The converted string.
 */
std::wstring ConvertToWString(std::string originalString);

/**
 * @brief Enumerates a list of currently connected devices, based on device type.
 *
 * @param deviceType The type of device to get a list of. Valid values are RIM_TYPEKEYBOARD
 * and RIM_TYPEMOUSE.
 * @return A variant containing a vector of keyboard or mouse device infos, depending on deviceType.
 */
std::variant<std::vector<KeyboardDeviceInfo>, std::vector<MouseDeviceInfo>> GetDeviceList(int deviceType);

}  // namespace NLSWIN