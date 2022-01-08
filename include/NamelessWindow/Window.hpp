/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 * @todo: Better way of handling closing the window. What should destroying a window look like in an OOP lang?
 * @todo: Destructor implementation. Currently the window doesn't actually close if you destroy it.
 * @todo: We will pass a RenderContext for each window constructor, because x11 windows need the glcontext
 * constructed first so that we can pass the appropriate visual.
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

namespace NLSWIN {

/*! @ingroup Common */
enum class WindowMode { FULLSCREEN = 0, BORDERLESS = 1, WINDOWED = 2, NO_PREFERENCE = 3 };

/*! Information about a physical monitor connected to the system. */
/*! @ingroup Common */
struct NLSWIN_API_PUBLIC MonitorInfo {
   const unsigned int horzResolution {0};     /*!< The horizontal pixel resolution of the monitor */
   const unsigned int verticalResolution {0}; /*!< The vertical pixel resolution of the monitor */
   const int16_t screenXCord {0};             /*!< X Coordinate location of the top-left hand corner of this
                                               * monitor. Each monitor exists in a "virtual screen" that
                                               * contains within it all monitors. This is relevant only
                                               * for multi-monitor setups.*/
   const int16_t screenYCord {0};             /*!< Y Coordinate location of the top-left hand corner of this
                                               * monitor. Each monitor exists in a "virtual screen" that
                                               * contains within it all monitors. This is relevant only
                                               * for multi-monitor setups.*/
   const std::string_view name {""};          /*!< Platform-specific name of the monitor. */
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
   std::optional<MonitorInfo> preferredMonitor; /*!< The preferred monitor to initialize the window to. If no
                                               monitor is selected, the first monitor will be preferred. */
};

/*!
 * @interface Window
 * @ingroup Common
 * @brief An application window that is displayed to the user.
 *
 * The window is the core of the NLSWIN library. It represents a single window drawn by the underlying
 * platform. Creation of windows is performed through the Create factory methods. Window close occurs upon
 * calling of the destructor. A window must be closed by the client - it is never closed by the NLSWIN
 * library.
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
};
}  // namespace NLSWIN