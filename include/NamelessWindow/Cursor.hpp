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

#include "InputDevice.hpp"
#include "NLSAPI.hpp"

namespace NLSWIN {

/**
 * @brief Represents the physical cursor seen on the screen.
 * @ingroup Common
 *
 * The cursor is impacted by all physical mouse devices connected to the system. If you wish to get
 * events from a specific physical device, you should use a RawMouse object.
 * @warning Currently, only a single Cursor per application is supported. Attempts to construct more than one
 * cursor will fail. Multiple-cursor setups controlled by distinct physical devices as a form of multiseating
 * may be supported in the future.
 */
class NLSWIN_API_PUBLIC Cursor : virtual public InputDevice {
   public:
   /**
    * @brief Constructs a new Cursor object.
    *
    * Currently only a single cursor is supported. Attempts to create more than one cursor will throw an
    * exception.
    *
    * @post A weak pointer to this object will be given to the EventDispatcher.
    * @throws PlatformInitializationException
    * @throws MultipleCursorException
    * @return A shared pointer to the newly constructed Cursor. Caller owns this resource and is
    * expected to manage its lifetime.
    */
   static std::shared_ptr<Cursor> Create();

   /*!
    * @brief Binds this cursor to a specified window, locking it within that window's bounds. It also sets the
    * window focus to the bound window.
    *
    * A cursor can only be bound to one window, so multiple calls of this method will switch the bound
    * window. Multiple attempts to bind the pointer to the window its already bound to does nothing. If the
    * window is not visible at the time of this call, the cursor will be bound as soon as the window becomes
    * visible.
    * @warning If the cursor is currently grabbed by another application, this request is discarded. If this
    * method is called before the window has been shown, this request is discarded. If the window is shown,
    * but completely obscured (eg, entirely hidden by another window), this request is discarded.
    * @param window The window to bind to.
    */
   virtual void BindToWindow(const Window *const window) noexcept = 0;
   /**
    * @brief Frees this cursor from any active window bind.
    *
    * If the cursor is not bound to any window, this request is discarded.
    */
   virtual void UnbindFromWindows() noexcept = 0;
   /**
    * Requests that the cursor be visible when inside any subscribed window, if it is not already.
    *
    * If the cursor is already visible, calling this method does nothing. */
   virtual void ShowCursor() noexcept = 0;
   /**
    * @brief Requests that the cursor be hidden when inside any subscribed window, if it is not already.
    *
    * If the cursor is already set to be hidden, calling this method does nothing. Note that this method will
    * not render the cursor invisible all the time, but only when the cursor enters the bounds of an
    * application's window.
    */
   virtual void HideCursor() noexcept = 0;

   virtual ~Cursor() = default;
};

}  // namespace NLSWIN