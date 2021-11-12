#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "NLSAPI.h"

namespace NLSWIN {

enum class NLSWIN_API_PUBLIC WindowMode { FULLSCREEN = 0, BORDERLESS = 1, WINDOWED = 2, NO_PREFERENCE = 3 };

struct NLSWIN_API_PUBLIC Monitor {
   unsigned int horzResolution     = 0;
   unsigned int verticalResolution = 0;
   unsigned int globalSpaceXCoord  = 0;
   unsigned int globalSpaceYCoord  = 0;
   std::string name;
};

struct NLSWIN_API_PUBLIC WindowProperties {
   unsigned int horzResolution = 860;
   unsigned int vertResolution = 480;
   unsigned int xCoordinate    = 0;
   unsigned int yCoordinate    = 0;
   unsigned int borderWidth    = 0;
   std::string windowName;
   WindowMode mode = WindowMode::NO_PREFERENCE;
   std::optional<Monitor> preferredMonitor;
};

class NLSWIN_API_PUBLIC Window {
   public:
   static std::unique_ptr<Window> CreateWindow(WindowProperties properties);
   static std::vector<Monitor> EnumerateMonitors();

   virtual int SetSomething(int value) = 0;
};
}  // namespace NLSWIN