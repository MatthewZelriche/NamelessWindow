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
 * @brief Nonspecific error that indicates an OS platform error occured. For example, if a specific library
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
class NLSWIN_API_PUBLIC InputDeviceFailureException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "An unspecified platform error has occured when constructing or attempting to use an input "
             "device.";
   }
};

/*!
 * @ingroup Common
 * @brief Only one Cursor is currently supported. Attempts to construct more than one Cursor will throw this
 * exception.
 * @see Cursor
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
 * @see EventListener
 */
class NLSWIN_API_PUBLIC EmptyEventQueueException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "An attempt was made to retrieve an event from a listener with no pending events.";
   }
};

/*!
 * @ingroup Common
 * @brief Thrown when initialization of a render context fails.
 */
class NLSWIN_API_PUBLIC RenderContextInitFailureException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "Underlying platform failed to initialize a valid render context.";
   }
};

/*!
 * @ingroup Common
 * @brief Thrown when an operation is performed on a context with an invalid state.
 */
class NLSWIN_API_PUBLIC InvalidRenderContextStateException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "A method call to a render context occured while it was an invalid state.";
   }
};

/*!
 * @ingroup Common
 * @brief Thrown when an operation was given a video mode that is considered invalid by the underlying
 * platform.
 */
class NLSWIN_API_PUBLIC InvalidVideoModeException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "A method was given a video mode deemed invalid by the platform.";
   }
};

}  // namespace NLSWIN