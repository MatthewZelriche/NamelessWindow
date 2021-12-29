#pragma once

#include <stdexcept>

#include "NLSAPI.h"

namespace NLSWIN {

class NLSWIN_API_PUBLIC BadMonitorException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "Attempted to construct window with bad monitor argument.";
   }
};

class NLSWIN_API_PUBLIC PlatformInitializationException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "The underlying platform API has encountered a serious error and has not "
             "been successfully initialized.";
   }
};

class NLSWIN_API_PUBLIC BadEventRegistrationException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "A platform error has occured while attempting to register for operating system events.";
   }
};

class NLSWIN_API_PUBLIC InputDeviceFailure : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "An unspecified platform error has occured when constructing or attempting to use an input "
             "device.";
   }
};

class NLSWIN_API_PUBLIC MultipleMasterPointerError : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "An attempt was made to instantiate a Master Pointer more than once. There can only be one "
             "master pointer per application";
   }
};

}  // namespace NLSWIN