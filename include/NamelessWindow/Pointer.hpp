/*!
 * @file Pointer.hpp
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup Common Public API
 * @brief Documentation for public API that clients directly interact with.
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Events/Event.hpp"
#include "Events/EventListener.hpp"
#include "NLSAPI.hpp"

namespace NLSWIN {
class Window;
class MasterPointer;

/*! Contains information about a specific pointer device. */
/*! @ingroup Common */
struct NLSWIN_API_PUBLIC PointerDeviceInfo {
   const std::string_view name {""};              /*! The name the OS has given this pointer device. */
   const uint32_t platformSpecificIdentifier {0}; /*! A platform specific identifier for this pointer device.
                                                     This cannot be relied upon to be unique across platforms
                                                     or even across different application launches. */
};

/*!
 * @interface Pointer
 * @ingroup Common
 * @brief Represents a specific physical pointer device (also known as mice).
 *
 * This interface defines how the client interacts with physical pointer devices connected to the system.
 * Construction of these objects is done through the Create factory method.
 * A Pointer must always be bound to one and only one window, due to how the underlying platforms handle
 * cursors. To be bound to a window means the pointer is restricted to within the bounds of that window. Since
 * a Pointer must always be bound to a window, certain events are unavailable to it, such as cursor enter and
 * cursor leave events.
 * Clients should use this class only if they are interested in events coming from a specific pointer device
 * only. Otherwise, utilizing the MasterPointer is a better choice. When using instances of this class, you
 * will likely want to bind and hide the master cursor.
 * @see MasterPointer
 */
class NLSWIN_API_PUBLIC Pointer : virtual public EventListener {
   public:
   /*!
    * @brief Construct a new instance representing a specific physical pointer specified by
    * PointerDeviceInfo.
    * @param device The specified physical pointer device you wish to listen for events on.
    * @param window The window this pointer should be bound to.
    * @post A weak pointer to this object will be given to the EventDispatcher.
    * @throws PlatformInitializationException
    * @return A shared pointer to the newly constructed Pointer. Caller owns this resource and is expected to
    * manage its lifetime.
    * @see EventDispatcher
    */
   static std::shared_ptr<Pointer> Create(PointerDeviceInfo device, const Window *const window);
   /*!
    * @brief Binds this pointer to a specified window, locking it within that window's bounds.
    *
    * A Pointer can only be bound to one window, so multiple calls of this method will switch the bound
    * window. Multiple attempts to bind the pointer to the window its already bound to does nothing. If the
    * window is not visible at the time of this call, the pointer will be bound as soon as the window becomes
    * visible.
    * @param window The window to bind to.
    */
   virtual void BindToWindow(const Window *const window) noexcept = 0;

   /**
    * @brief Gets a list of all detected pointer devices.
    *
    * @return A vector of PointerDeviceInfos, each elementing containing information on a single pointer
    * device.
    */
   [[nodiscard]] static std::vector<PointerDeviceInfo> EnumeratePointers() noexcept;

   virtual ~Pointer() = default;
};

/*!
 * @interface MasterPointer
 * @ingroup Common
 * @brief Represents the singular OS cursor and receives events from all physical pointer devices.
 *
 * This interface defines how the client interacts with OS-provided cursor. Only one MasterPointer can exist
 * at a time, and the NLSWIN library will automatically construct it. The singular instance of the
 * MasterPointer can be accessed through a getter method. Unlike Pointer, MasterPointer need not be bound to a
 * window, and can receive events from any window.
 * If clients are interested in getting event data from a specific pointer, they should consider the Pointer
 * interface instead.
 * @warning Do not attempt to subclass MasterPointer yourself. Only one MasterPointer can exist, and the
 * NLSWIN library extends to be able to construct the only instance.
 * @see Pointer
 */
class NLSWIN_API_PUBLIC MasterPointer : virtual public Pointer {
   public:
   /*!
    * @brief Get the singular MasterPointer instance.
    * @throws PlatformInitializationException
    * @return A reference to the MasterPointer instance.
    */
   static MasterPointer &GetMasterPointer();
   /*! Unbinds the MasterPointer from any window. Calling this method when MasterPointer is not bound to a
    * window does nothing. This is called automatically whenever the bound window loses focus. */
   virtual void UnbindFromWindow() noexcept = 0;
   /**
    * Requests that the cursor be visible when inside a window, if it is not already. If the cursor is
    * already visible, calling this method does nothing. */
   virtual void ShowCursor() noexcept = 0;
   /**
    * @brief Requests that the cursor be hidden when inside a window, if it is not already. If the cursor is
    * already set to be hidden, calling this method does nothing.
    *
    * Note that this method will not render the cursor invisible all the time, but only when the cursor enters
    * the bounds of an application's window.
    */
   virtual void HideCursor() noexcept = 0;

   protected:
   /**
    * @brief Construct a new Master Pointer object
    * @warning Do not attempt to subclass MasterPointer yourself. Only one MasterPointer can exist, and the
    * NLSWIN library extends to be able to construct the only instance.
    */
   MasterPointer() = default;
   virtual ~MasterPointer() = default;
};

}  // namespace NLSWIN