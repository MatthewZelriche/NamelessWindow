#pragma once

#include <string>

#include "NLSAPI.h"

namespace NLSWIN {
struct KeyboardDeviceInfo {
   std::string name;
   uint32_t platformSpecificIdentifier;
};

}  // namespace NLSWIN