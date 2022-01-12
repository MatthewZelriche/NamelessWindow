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
#include <string>

#include "Events/EventListener.hpp"
#include "NLSAPI.hpp"
#include "Window.hpp"

namespace NLSWIN {

/*! Contains information about a specific pointer device. */
/*! @ingroup Common */
struct NLSWIN_API_PUBLIC MouseDeviceInfo {
   const std::string_view name {""};              /*! The name the OS has given this pointer device. */
   const uint32_t platformSpecificIdentifier {0}; /*! A platform specific identifier for this pointer device.
                                                     This cannot be relied upon to be unique across platforms
                                                     or even across different application launches. */
};

/*!
 * @ingroup Common
 * @brief Represents a specific physical mouse device that can read raw device data.
 *
 * This interface defines how the client interacts with physical mice connected to the system.
 * Construction of these objects is done through the Create factory method.
 * A RawMouse must always be bound to one and only one window, due to how the underlying platforms handle
 * cursors. To be bound to a window means the pointer is restricted to within the bounds of that window. Since
 * a Pointer must always be bound to a window, certain events are unavailable to it, such as cursor enter and
 * cursor leave events.
 * Clients should use this class only if they are interested in events coming from a specific physical mouse
 * only. Otherwise, cursor events can be accessed through a Window's event queue.
 * @see Window
 */
class NLSWIN_API_PUBLIC RawMouse : virtual public EventListener {
   public:
   /*!
    * @brief Construct a new instance representing a specific physical mouse specified by
    * MouseDeviceInfo.
    * @param device The specified physical mouse you wish to listen for events on.
    * @post A weak pointer to this object will be given to the EventDispatcher.
    * @throws PlatformInitializationException
    * @return A shared pointer to the newly constructed mouse device. Caller owns this resource and is
    * expected to manage its lifetime.
    * @see EventDispatcher
    */
   static std::shared_ptr<RawMouse> Create(MouseDeviceInfo device);

   /**
    * @brief Gets a list of all detected mice.
    *
    * @return A vector of MouseDeviceInfos, each elementing containing information on a single mouse.
    */
   [[nodiscard]] static std::vector<MouseDeviceInfo> EnumeratePointers() noexcept;
};
}  // namespace NLSWIN