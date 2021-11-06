#pragma once
#include <memory>

#include "NLSAPI.h"

namespace NLSWIN {
class NLSWIN_API_PUBLIC Window {
   public:
   static std::unique_ptr<Window> CreateWindow();

   virtual int SetSomething(int value) = 0;
};
}  // namespace NLSWIN