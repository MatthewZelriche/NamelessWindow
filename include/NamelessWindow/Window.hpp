/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 * @todo: Better way of handling closing the window. What should destroying a window look like in an OOP lang?
 * @todo: Destructor implementation. Currently the window doesn't actually close if you destroy it.
 *
 * @addtogroup Common Public API
 * @brief Documentation for public API that clients directly interact with.
 */
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Events/EventListener.hpp"
#include "NLSAPI.hpp"
#include "Pointer.hpp"

namespace NLSWIN {

using WindowID = uint32_t;
class KeyboardDeviceInfo;
class Keyboard;
class RawPointer;

/*! @ingroup Common */
enum class WindowMode { FULLSCREEN = 0, BORDERLESS = 1, WINDOWED = 2, NO_PREFERENCE = 3 };

/*! Information about a physical monitor connected to the system. */
/*! @ingroup Common */
struct NLSWIN_API_PUBLIC Monitor {
   const unsigned int horzResolution {0};     /*!< The horizontal pixel resolution of the monitor */
   const unsigned int verticalResolution {0}; /*!< The vertical pixel resolution of the monitor */
   const int16_t globalSpaceXCoord {0};       /*!< X Coordinate location of the top-left hand corner of this
                                               * monitor. Each monitor exists in a "virtual screen" that
                                               * contains within it all monitors. This is relevant only
                                               * for multi-monitor setups.*/
   const int16_t globalSpaceYCoord {0};       /*!< Y Coordinate location of the top-left hand corner of this
                                               * monitor. Each monitor exists in a "virtual screen" that
                                               * contains within it all monitors. This is relevant only
                                               * for multi-monitor setups.*/
   const std::string name {""};               /*!< Platform-specific name of the monitor. */
};

/*!
 * @brief Properties that can be used to configure the initial state of a newly created window.
 * @ingroup Common
 *
 * Note that these properties are only requests, and that they may be ignored by the underlying platform
 * during actual window creation. */
struct NLSWIN_API_PUBLIC WindowProperties {
   unsigned int horzResolution {860};           /*!< Initial horizontal resolution, in pixels. */
   unsigned int vertResolution {480};           /*!< Initial vertical resolution, in pixels. */
   unsigned int xCoordinate {0};                /*!< The X coordinate for the top-right corner of the window,
                                                 * relative to the given monitor's global space coordinates. */
   unsigned int yCoordinate {0};                /*!< The Y coordinate for the top-right corner of the window,
                                                 * relative to the given monitor's global space coordinates. */
   unsigned int borderWidth {0};                /*!< Thickness of the border decorations. */
   bool isUserResizable {true};                 /*!< Whether the application user should be allowed to
                                                 * manually resize the window. Note that the application
                                                 * itself is still free to resize the window. */
   WindowMode mode {WindowMode::NO_PREFERENCE}; /*!< The window mode to start in. */
   std::string windowName;                      /*!< The name to be displayed in the window's titlebar. */
   std::optional<Monitor> preferredMonitor;     /*!< The preferred monitor to initialize the window to. If no
                                                   monitor is selected, the first monitor will be preferred. */
};

/*!
 * @interface Window
 * @ingroup Common
 * @brief An application window that is displayed to the user.
 *
 * The window is the core of the NLSWIN library. It represents a single window drawn by the underlying
 * platform. Creation of windows is performed through the Create factory methods.
 * @see WindowProperties
 */
class NLSWIN_API_PUBLIC Window : virtual public EventListener {
   public:
   /*!
    * @brief Construct a new window with default properties.
    * @post A weak pointer to this object will be given to the EventDispatcher.
    * @throws PlatformInitializationException
    * @throws BadMonitorException
    * @return A shared pointer to the newly constructed Window. Caller owns this resource and is expected to
    * manage its lifetime.
    * @see EventDispatcher
    */
   static std::shared_ptr<Window> Create();
   /*!
    * @brief Construct a new window with client-specified properties.
    * @param properties The client-specified WindowProperties for use in initialization of the window.
    * @post A weak pointer to this object will be given to the EventDispatcher.
    * @throws PlatformInitializationException
    * @throws BadMonitorException
    * @return A shared pointer to the newly constructed Window. Caller owns this resource and is expected to
    * manage its lifetime.
    * @see EventDispatcher
    */
   static std::shared_ptr<Window> Create(WindowProperties properties);
   /*!
    * @brief Request that the window be drawn fullscreen.
    * @param borderless Whether the window should be drawn in borderless fullscreen. This does not impact
    * whether windows in the WINDOWMODE::Windowed state are drawn with borders.
    */
   virtual void SetFullscreen(bool borderless = true) noexcept = 0;
   /*! Request that the window be drawn as a regular window, not in fullscreen. */
   virtual void SetWindowed() noexcept = 0;
   /*!
    * @brief Request that the monitor be drawn at a new position.
    *
    * Note that this does not account for global space monitor offsets. The arguments correspond to the
    * virtual screen coordinates, not the monitor coordinates.
    *
    * @param newX The new X coordinate for the top-left hand of the window.
    * @param newY The new Y coordinate for the top-left hand of the window.
    */
   virtual void RepositionWindow(uint32_t newX, uint32_t newY) noexcept = 0;
   /*!
    * @brief Request that the monitor be drawn with new dimensions.
    *
    * Note that some platforms will ignore this if they handle window sizing in a specific fashion (eg, tiling
    * window managers).
    *
    * @param width The new width of the monitor.
    * @param height The new height of the monitor.
    */
   virtual void Resize(uint32_t width, uint32_t height) noexcept = 0;
   /**
    * @brief Whether a request to close the window has been made.
    *
    * @return True if a request was made, false otherwise.
    */
   [[nodiscard]] virtual bool RequestedClose() const noexcept = 0;
   /*! Gets the current mode. */
   [[nodiscard]] virtual WindowMode GetWindowMode() const noexcept = 0;
   /*! Gets a unique ID for the window. */
   [[nodiscard]] virtual WindowID GetWindowID() const noexcept = 0;
   /*! Gets the current width of the window, in pixels. */
   [[nodiscard]] virtual unsigned int GetWindowWidth() const noexcept = 0;
   /*! Gets the current height of the window, in pixels. */
   [[nodiscard]] virtual unsigned int GetWindowHeight() const noexcept = 0;

   /**
    * @brief Gets a list of all detected monitors.
    *
    * @return A vector of Monitors, each elementing containing information on a single monitor.
    */
   [[nodiscard]] static std::vector<Monitor> EnumerateMonitors() noexcept;

   virtual ~Window() = default;
};
}  // namespace NLSWIN