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

}  // namespace NLSWIN