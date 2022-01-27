/*!
 * @file Exceptions.hpp
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup Common Public API
 * @brief Documentation for public API that clients directly interact with.
 */
#pragma once
#include <stdexcept>

#include "NLSAPI.hpp"

namespace NLSWIN {

/*!
 * @ingroup Common
 * @brief Thrown when a MonitorInfo is passed to a Window constructor, but the specified Monitor cannot be
 * found.
 *
 * @see MonitorInfo
 */
class NLSWIN_API_PUBLIC BadMonitorException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "Attempted to construct window with bad monitor argument.";
   }
};

/*!
 * @ingroup Common
 * @brief Nonspecific error that indicates an OS platform error occured. For example, if a cpecific library
 * extension is not installed or supported.
 */
class NLSWIN_API_PUBLIC PlatformInitializationException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "The underlying platform API has encountered a serious error and has not "
             "been successfully initialized.";
   }
};

/*!
 * @ingroup Common
 * @brief Indicates when the OS has failed to initialize or access an input device.
 */
class NLSWIN_API_PUBLIC InputDeviceFailure : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "An unspecified platform error has occured when constructing or attempting to use an input "
             "device.";
   }
};

/*!
 * @ingroup Common
 * @brief Only one Master Pointer can exist at a time.
 *
 * Usually this exception would occur because an attempt was made by the client to subclass MasterPointer
 * @see MasterPointer
 */
class NLSWIN_API_PUBLIC MultipleCursorException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "An attempt was made to instantiate a Cursor more than once. There can only be one "
             "Cursor.";
   }
};

/*!
 * @ingroup Common
 * @brief Thrown when there is an attempt to pop an event from an empty EventListener queue.
 *
 * @see EventListener
 */
class NLSWIN_API_PUBLIC EmptyEventQueueException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "An attempt was made to retrieve an event from a listener with no pending events.";
   }
};

}  // namespace NLSWIN