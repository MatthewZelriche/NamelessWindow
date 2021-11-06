#pragma once
#include "NamelessWindow/Window.hpp"

namespace NLSWIN {
class NLSWIN_API_PRIVATE WindowX11 : public Window {
   public:
   WindowX11();
   virtual int SetSomething(int value) override;
};
}  // namespace NLSWIN
