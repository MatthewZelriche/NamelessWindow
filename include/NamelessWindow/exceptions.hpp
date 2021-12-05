#pragma once

#include <stdexcept>

namespace NLSWIN {

class BadMonitorException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "Attempted to construct window with bad monitor argument.";
   }
};

}  // namespace NLSWIN