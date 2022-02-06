#pragma once

#include <string>

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

}  // namespace NLSWIN