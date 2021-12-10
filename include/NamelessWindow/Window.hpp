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
   unsigned int horzResolution {0};
   unsigned int verticalResolution {0};
   unsigned int globalSpaceXCoord {0};
   unsigned int globalSpaceYCoord {0};
   std::string name;
};

struct NLSWIN_API_PUBLIC WindowProperties {
   unsigned int horzResolution {860};
   unsigned int vertResolution {480};
   unsigned int xCoordinate {0};
   unsigned int yCoordinate {0};
   unsigned int borderWidth {0};
   WindowMode mode {WindowMode::NO_PREFERENCE};
   std::string windowName;
   std::optional<Monitor> preferredMonitor;
};

class NLSWIN_API_PUBLIC Window {
   private:
   class Impl;
   std::shared_ptr<Impl> m_pImpl {nullptr};
   friend class Keyboard;

   public:
   Window();
   Window(WindowProperties properties);
   ~Window();
   void SetFullscreen(bool borderless = true) noexcept;
   void SetWindowed() noexcept;
   void Close() noexcept;
   [[nodiscard]] bool RequestedClose() const noexcept;
   [[nodiscard]] bool HasEvent() const noexcept;
   Event GetNextEvent();
   [[nodiscard]] WindowMode GetWindowMode() const noexcept;

   [[nodiscard]] static std::vector<Monitor> EnumerateMonitors() noexcept;
};
}  // namespace NLSWIN