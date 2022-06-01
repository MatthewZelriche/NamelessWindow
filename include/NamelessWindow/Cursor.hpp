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

#include "NLSAPI.hpp"
#include "SubscribableInputDevice.hpp"

namespace NLSWIN {

/**
 * @brief Represents the physical cursor seen on the screen.
 * @ingroup Common
 *
 * This class lets you query information related to the physical cursor that is displayed to the user on the
 * screen. The cursor is impacted by all physical mouse devices connected to the system. If you wish to get
 * raw information from a specific physical mouse device, you should instead use a RawMouse object.
 *
 * The Cursor can return the following event types: MouseButtonEvent, MouseScrollEvent, MouseMovementEvent,
 * RawMouseDeltaMovementEvent, MouseEnterEvent, MouseLeaveEvent.
 * @warning Currently, only a single Cursor per application is supported. Attempts to construct more than one
 * cursor will fail. Multiple-cursor setups controlled by distinct physical devices as a form of multiseating
 * may be supported in the future.
 */
class NLSWIN_API_PUBLIC Cursor : virtual public NLSWIN::EventListener {
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
    * @return A shared pointer to the newly constructed instance of this class. Caller owns this resource and
    * is expected to manage its lifetime.
    */
   static std::shared_ptr<Cursor> Create();

   /*!
    * @brief Confines this cursor to a specified window, locking it within that window's client area. The
    * window also becomes focused.
    *
    * After calling this method, the cursor will be confined to the specified window's client area. If focus
    * on that window is lost, the cursor will be temporarily freed, until focus returns to the bound window.
    * A cursor can only be bound to one window, so multiple calls of this method will switch the bound
    * window. Multiple attempts to bind the curser to the window its already bound to does nothing.
    * @warning If the cursor is currently grabbed by another application, this request may be discarded. If
    * this method is called before the window has been shown, behavior is undefined. If the window is
    * destroyed while the cursor is confined to it, the cursor is freed and no longer confined to any window.
    *
    * @param window The window to bind to.
    */
   virtual void Confine(Window* window) noexcept = 0;

   /**
    * @brief Frees this cursor from any active window confinement.
    *
    * If the cursor is not bound to any window, this request is discarded.
    */
   virtual void Free() noexcept = 0;

   /**
    * Requests that the cursor icon be rendered when inside any application window.
    *
    * If the cursor is already visible, this request is discarded.
    */
   virtual void Show() noexcept = 0;

   /**
    * @brief Requests that the cursor icon be hidden when inside any application window.
    *
    * If the cursor is already set to be hidden, this request is discarded. Note that this method will
    * not render the cursor invisible all the time, but only when the cursor enters the client area of an
    * application window.
    */
   virtual void Hide() noexcept = 0;

   virtual ~Cursor() = default;
};

}  // namespace NLSWIN