#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "EventListener.hpp"
#include "NLSAPI.h"
#include "Pointer.hpp"

namespace NLSWIN {

using WindowID = uint32_t;
class KeyboardDeviceInfo;
class Keyboard;
class RawPointer;

enum class NLSWIN_API_PUBLIC WindowMode { FULLSCREEN = 0, BORDERLESS = 1, WINDOWED = 2, NO_PREFERENCE = 3 };

struct NLSWIN_API_PUBLIC Monitor {
   const unsigned int horzResolution {0};
   const unsigned int verticalResolution {0};
   const int16_t globalSpaceXCoord {0};
   const int16_t globalSpaceYCoord {0};
   const std::string name {""};
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

class NLSWIN_API_PUBLIC Window : virtual public EventListener {
   public:
   static std::shared_ptr<Window> Create();
   static std::shared_ptr<Window> Create(WindowProperties properties);
   ~Window();
   virtual void SetFullscreen(bool borderless = true) noexcept = 0;
   virtual void SetWindowed() noexcept = 0;
   virtual void Close() noexcept = 0;
   virtual Pointer &GetMasterPointer() = 0;
   [[nodiscard]] virtual bool RequestedClose() const noexcept = 0;
   [[nodiscard]] virtual WindowMode GetWindowMode() const noexcept = 0;
   [[nodiscard]] virtual WindowID GetWindowID() const noexcept = 0;
   [[nodiscard]] virtual unsigned int GetWindowWidth() const noexcept = 0;
   [[nodiscard]] virtual unsigned int GetWindowHeight() const noexcept = 0;

   [[nodiscard]] static std::vector<Monitor> EnumerateMonitors() noexcept;
};
}  // namespace NLSWIN