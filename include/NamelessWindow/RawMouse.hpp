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

/*! Contains information about a specific mouse device. */
/*! @ingroup Common */
struct NLSWIN_API_PUBLIC MouseDeviceInfo {
   const std::string name {""};              /*! The name the OS has given this mouse device. */
   const uint64_t platformSpecificIdentifier {0}; /*! A platform specific identifier for this mouse device.
                                                     This cannot be relied upon to be unique across platforms
                                                     or even across different application launches. */
};

/*!
 * @ingroup Common
 * @brief Represents a specific physical mouse device that can read raw device data.
 *
 * This class defines how the client interacts with physical mice connected to the system.
 * A RawMouse has no relation to any window. It receives events regardless of where the cursor is, even if
 * events are being sent to windows not created by this application. This class can only return the following
 * events: RawMouseScrollEvent, RawMouseButtonEvent, RawMouseDeltaMovementEvent.
 * Clients should use this class only if they are interested in events coming from a specific physical mouse
 * only. Otherwise, you likely want to use Cursor instead.
 * @see Cursor
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
    */
   static std::shared_ptr<RawMouse> Create(MouseDeviceInfo device);

   /**
    * @brief Gets a list of all currently detected mice.
    *
    * @return A vector of MouseDeviceInfos, each elementing containing information on a single mouse.
    */
   [[nodiscard]] static std::vector<MouseDeviceInfo> EnumeratePointers() noexcept;
};
}  // namespace NLSWIN