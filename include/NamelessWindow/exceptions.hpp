#pragma once

#include <stdexcept>

namespace NLSWIN {

class BadScreenException : public std::exception {
   public:
   virtual const char* what() const noexcept override {
      return "Attempted to construct window with bad screen argument.";
   }
};

}  // namespace NLSWIN