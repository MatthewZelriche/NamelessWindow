#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "NLSAPI.h"

namespace NLSWIN {

class Keyboard;

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
   private:
   class WindowImpl;
   friend class Keyboard;
   std::shared_ptr<WindowImpl> m_pImpl {nullptr};

   public:
   Window();
   ~Window();
   Window(WindowProperties properties);
   static std::vector<Monitor> EnumerateMonitors();
   void SetFullscreen(bool borderless = true);
   void SetWindowed();
   void Close();
   bool RequestedClose() const;
   bool HasEvent();
   Event GetNextEvent();

   WindowMode GetWindowMode() const;
};
}  // namespace NLSWIN