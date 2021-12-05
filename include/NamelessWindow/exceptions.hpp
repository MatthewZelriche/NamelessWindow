#pragma once

#include <stdexcept>

namespace NLSWIN {

class BadMonitorException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "Attempted to construct window with bad monitor argument.";
   }
};

class InvalidEventDispatcherException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "Attempted to use the Event Dispatcher while it is in an invalid state.";
   }
};

}  // namespace NLSWIN