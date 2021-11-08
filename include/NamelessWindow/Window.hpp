#pragma once
#include <memory>
#include <string>
#include <vector>

#include "NLSAPI.h"

namespace NLSWIN {

struct NLSWIN_API_PUBLIC Screen {
   unsigned int horzResolution     = 0;
   unsigned int verticalResolution = 0;
   unsigned int xCoord             = 0;
   unsigned int yCoord             = 0;
   std::string name;
};

class NLSWIN_API_PUBLIC Window {
   public:
   static std::unique_ptr<Window> CreateWindow();
   static std::vector<Screen> EnumerateScreens();

   virtual int SetSomething(int value) = 0;
};
}  // namespace NLSWIN