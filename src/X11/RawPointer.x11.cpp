#include "RawPointer.x11.hpp"

#include <cstring>

using namespace NLSWIN;

std::vector<PointerDeviceInfo> RawPointer::EnumeratePointers() noexcept {
   return EnumerateDevicesX11<PointerDeviceInfo>(XCB_INPUT_DEVICE_TYPE_SLAVE_POINTER);
}