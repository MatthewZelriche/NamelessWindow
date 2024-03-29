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

#include "NLSAPI.hpp"
#include "SubscribableInputDevice.hpp"

namespace NLSWIN {
/*! Contains information about a specific keyboard device. */
/*! @ingroup Common */
struct NLSWIN_API_PUBLIC KeyboardDeviceInfo {
   const std::string name {""};                   /*! The name the OS has given this keyboard device. */
   const uint64_t platformSpecificIdentifier {0}; /*! A platform specific identifier for this keyboard device.
                                                     This cannot be relied upon to be unique across platforms
                                                     or even across different application launches. */
};

/**
 * @ingroup Common
 * @brief Represents one or many physical keyboards.
 *
 * Defines how the client interacts with physical keyboard devices connected to the system.
 * Capable of receiving the following events: KeyEvent, CharacterEvent.
 * @todo Handle device connect/disconnect events. 
 */
class NLSWIN_API_PUBLIC Keyboard : virtual public SubscribableInputDevice {
   public:
   /**
    * @brief Construct a new instance of a "master" Keyboard that will listen for events from all connected
    * physical keyboard devices.
    *
    * Use this method if you have no interest in distinguishing keyboard events based on the physical device
    * from which they originated.
    * @post A weak pointer to this object will be given to the EventDispatcher.
    * @throws PlatformInitializationException
    * @return A shared pointer to the newly constructed Keyboard. Caller owns this resource and is expected to
    * manage its lifetime.
    */
   static std::shared_ptr<Keyboard> Create();
   /**
    * @brief Construct a new instance representing a specific physical keyboard specified by
    * KeyboardDeviceInfo.
    *
    * Use this method if you wish to receive events from a specific physical keyboard device only.
    * Otherwise, clients should use Create() instead.
    * @param device The specified physical keyboard device you wish to listen for events on.
    * @post A weak pointer to this object will be given to the EventDispatcher.
    * @throws PlatformInitializationException
    * @return A shared pointer to the newly constructed Keyboard. Caller owns this resource and is expected to
    * manage its lifetime.
    */
   static std::shared_ptr<Keyboard> Create(KeyboardDeviceInfo device);

   /**
    * @brief Gets a list of all currently detected keyboard devices.
    *
    * @return A vector of KeyboardDeviceInfos, each elementing containing information on a single keyboard
    * device.
    */
   [[nodiscard]] static std::vector<KeyboardDeviceInfo> EnumerateKeyboards() noexcept;

   virtual ~Keyboard() = default;
};
}  // namespace NLSWIN