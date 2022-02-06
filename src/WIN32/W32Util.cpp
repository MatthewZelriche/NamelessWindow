#include "W32Util.hpp"

#include <windows.h>

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