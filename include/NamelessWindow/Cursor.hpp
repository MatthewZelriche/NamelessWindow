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
 * This class lets you query information related to the physical cursor that is displayed to the user on the screen. 
 * The cursor is impacted by all physical mouse devices connected to the system. If you wish to get
 * raw information from a specific physical mouse device, you should instead use a RawMouse object. 
 * 
 * The Cursor can return the following event types: MouseButtonEvent, MouseScrollEvent, MouseMovementEvent, 
 * RawMouseDeltaMovementEvent, MouseEnterEvent, MouseLeaveEvent.
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
    * @return A shared pointer to the newly constructed instance of this class. Caller owns this resource and is
    * expected to manage its lifetime.
    */
   static std::shared_ptr<Cursor> Create();

   /*!
    * @brief Binds this cursor to a specified window, locking it within that window's bounds.
    *
    * After calling this method, the cursor will be confined to the specified window's area. If focus 
    * on that window is lost, the cursor will be temporarily freed, until focus returns to the bound window.
    * A cursor can only be bound to one window, so multiple calls of this method will switch the bound
    * window. Multiple attempts to bind the pointer to the window its already bound to does nothing.
    * @warning If the cursor is currently grabbed by another application, this request is discarded. If this
    * method is called before the window has been shown, behavior is undefined.
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