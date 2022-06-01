/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
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
enum class WindowMode { FULLSCREEN = 0, WINDOWED = 1 };

/*! @ingroup Common */
struct Rect {
   int x {0};
   int y {0};
   int width {0};
   int height {0};
};

/*! @ingroup Common */
struct Point {
   int x {0};
   int y {0};
};

/*! Information about a video mode supported by a specific Monitor.*/
/*! @ingroup Common */
struct NLSWIN_API_PUBLIC VideoMode {
   const int horzResolution {0};                      /*!< The horizontal pixel resolution of this mode. */
   const int vertResolution {0};                      /*!< The vertical pixel resolution of this mode. */
   const float refreshRate {0};                       /*!< The refresh rate of this mode. */
   const unsigned int platformSpecificIdentifier {0}; /*!< Platform-specific ID, for internal use only. */
};

/*! Information about a physical monitor connected to the system. */
/*! @ingroup Common */
struct NLSWIN_API_PUBLIC MonitorInfo {
   const int horzResolution {0};       /*!< The horizontal pixel resolution of the monitor */
   const int verticalResolution {0};   /*!< The vertical pixel resolution of the monitor */
   const long int screenXCord {0};     /*!< X Coordinate location of the top-left hand corner of this
                                        * monitor. Each monitor exists in a "virtual screen" that
                                        * contains within it all monitors. This is relevant only
                                        * for multi-monitor setups.*/
   const long int screenYCord {0};     /*!< Y Coordinate location of the top-left hand corner of this
                                        * monitor. Each monitor exists in a "virtual screen" that
                                        * contains within it all monitors. This is relevant only
                                        * for multi-monitor setups.*/
   const std::string name {""};        /*!< Platform-specific name of the monitor. */
   const std::vector<VideoMode> modes; /*!< A list of video modes supported by this monitor. */
};

/**
 * @brief Requests support from a window for certain OpenGL configurations, such as
 * disabling double buffering, size of the depth buffer, etc.
 * @ingroup Common
 *
 * @warning Absolutely no checking for correctness of values is performed on this struct. Invalid values being
 * passed to a window will result in undefined behavior.
 */
struct NLSWIN_API_PUBLIC GLConfiguration {
   int glDepthSize {24};
   int redBitSize {8};
   int greenBitSize {8};
   int blueBitSize {8};
};

/*!
 * @brief Properties that can be used to configure the initial state of a newly created window.
 * @ingroup Common
 *
 * Note that these properties are only requests, and that they may be ignored by the underlying platform
 * during actual window creation. */
struct NLSWIN_API_PUBLIC WindowProperties {
   unsigned int horzResolution {860};      /*!< Initial horizontal resolution, in pixels. */
   unsigned int vertResolution {480};      /*!< Initial vertical resolution, in pixels. */
   unsigned int xCoordinate {0};           /*!< The X coordinate for the top-right corner of the window,
                                            * relative to the given monitor's virtual screen coordinates. */
   unsigned int yCoordinate {0};           /*!< The Y coordinate for the top-right corner of the window,
                                            * relative to the given monitor's virtual screen coordinates. */
   bool startBorderless {false};           /*!< Whether a titlebar/border should be drawn around the window.*/
   WindowMode mode {WindowMode::WINDOWED}; /*!< The window mode to start in. */
   std::string windowName;                 /*!< The name to be displayed in the window's titlebar. */
   std::optional<GLConfiguration> glConfig;     /*! A custom OpenGL configuration that this window should
                                                 * support. @todo: Not currently implemented correctly. */
   std::optional<MonitorInfo> preferredMonitor; /*!< The preferred monitor to initialize the window to. If no
                                               monitor is selected, the primary monitor will be preferred. */
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
    * @warning Show must be called before the window will be drawn on the screen after creation.
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
    * @warning Show must be called before the window will be drawn on the screen after creation.
    */
   static std::shared_ptr<Window> Create(WindowProperties properties);

   /**
    * @brief Draw the window onto the screen.
    *
    * This method blocks until the window has been successfully drawn, if necessary. During this time,
    * events are automatically dispatched as if the client had called EventBus::PollEvents().
    */
   virtual void Show() = 0;
   /**
    * @brief Stop drawing the window to the screen. The window will be hidden from the user.
    *
    * This method blocks until the window has been successfully hidden, if necessary. During this time,
    * events are automatically dispatched as if the client had called EventBus::PollEvents().
    */
   virtual void Hide() = 0;

   /**
    * @brief Requests that the window be drawn without border decorations.
    *
    * Fullscreen windows are always borderless. Attempts to call this method while fullscreen will be
    * discarded.
    */
   virtual void EnableBorderless() noexcept = 0;

   /**
    * @brief Requests that the window be drawn with default border decorations.
    *
    * Fullscreen windows are always borderless. Attempts to call this method while fullscreen will be
    * discarded. The border decorations that will be drawn are whatever the default decorations provided by
    * the Operating System and/or Window Manager are.
    */
   virtual void DisableBorderless() noexcept = 0;

   /*!
    * @brief Request that the window be drawn fullscreen.
    * @throws InvalidVideoModeException
    *
    * Resizes and repositions the window so that the client area encompasses the entire screen of the monitor
    * that it is currently inhabiting. Border decorations are automatically disabled while in this mode. The
    * monitor will have its resolution set to match the current resolution of the window. If such a video mode
    * is not supported by the monitor, an exception is thrown. Attempts to call this method while the window
    * is already fullscreen are discarded.
    * Must be called while the window is shown, else undefined behavior occurs.
    */
   virtual void SetFullscreen() = 0;

   /*! @brief Request that the window be drawn as a regular window, not in fullscreen.
    *
    * Transitioning from fullscreen mode into windowed mode will restore the previous video mode for the
    * monitor. An attempt is made to retain the positioning of the window, but the OS/Window Manager may
    * adjust the final positioning to accomodate for window borders. Must be called while the window is shown,
    * else undefined behavior occurs.
    */
   virtual void SetWindowed() noexcept = 0;

   /*!
    * @brief Request that the monitor be drawn at a new position.
    *
    * Places the top-left corner of the window's client area at the specified new coordinates. The arguments
    * correspond to the virtual screen coordinates, not the monitor coordinates. This means if you wish to
    * position a window onto a new monitor, you must account for monitor offsets yourself.
    * The underlying platform reserves the right to modify a request to reposition a window, for example to
    * accomodate window borders. Calls to this method while fullscreen are discarded.
    * @todo: Way of allowing calls to this method only to swap the monitor that the window is displayed on.
    *
    * @param newX The new X coordinate for the top-left hand of the window's client area.
    * @param newY The new Y coordinate for the top-left hand of the window's client area.
    */
   virtual void Reposition(uint32_t newX, uint32_t newY) noexcept = 0;

   /*!
    * @brief Request that the window's client area be drawn with new dimensions.
    * @throws InvalidVideoModeException
    *
    * Note that some platforms will ignore this if they handle window sizing in a specific fashion (eg, tiling
    * window managers).
    * If the window is fullscreen, a call to this method will attempt to set the video mode of the monitor to
    * the newly requested resolution. If that resolution is not supported, an exception is thrown.
    *
    * @param width The new width of the window, in pixels.
    * @param height The new height of the window, in pixels.
    */
   virtual void Resize(uint32_t width, uint32_t height) = 0;

   /*!
    * @brief Requests that the window be minimized.
    * @throws InvalidVideoModeException
    *
    * @param restoreVideoMode Whether the previous video mode should be restored after the window is
    * minimized.
    */
   virtual void Minimize(bool restoreVideoMode = false) = 0;

   /*! @brief Sets this window as the active window. */
   virtual void Focus() noexcept = 0;

   /**
    * @brief Whether a request to close the window has been made.
    *
    * Note that this only means the underlying platform has requested the window be destroyed - the window
    * won't actually be destroyed automatically. It is the application's responsibility to destroy the window.
    * An example of a close request being triggered is when the application user clicks the X button to close
    * the window.
    * @return True if a request was made, false otherwise.
    */
   [[nodiscard]] virtual bool RequestedClose() const noexcept = 0;
   /*! Gets the current mode. */
   [[nodiscard]] virtual WindowMode GetWindowMode() const noexcept = 0;
   /*! Gets the current width of the window, in pixels. */
   [[nodiscard]] virtual unsigned int GetWindowWidth() const noexcept = 0;
   /*! Gets the current height of the window, in pixels. */
   [[nodiscard]] virtual unsigned int GetWindowHeight() const noexcept = 0;
   /*! Gets the current position of the top left corner of the window's client area. */
   [[nodiscard]] virtual Point GetWindowPos() const noexcept = 0;
   /** Query whether the window is currently in a borderless state. */
   [[nodiscard]] virtual bool IsBorderless() const noexcept = 0;
   /*! Gets a platform-independent numeric value that represents this window.*/
   [[nodiscard]] virtual WindowID GetGenericID() const noexcept { return m_genericID; }

   /**
    * @brief Gets a list of all detected monitors.
    * @throws PlatformInitializationException
    * @return A vector of MonitorInfos, each elementing containing information on a single monitor.
    */
   [[nodiscard]] static std::vector<MonitorInfo> EnumerateMonitors();

   virtual ~Window() = default;

   protected:
   void NewID() {
      static WindowID ID = 1;
      m_genericID = ID++;
   }

   private:
   WindowID m_genericID {0};
};

}  // namespace NLSWIN