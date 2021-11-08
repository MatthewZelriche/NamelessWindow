#pragma once
#include <vector>

#include "NamelessWindow/Window.hpp"

class xcb_connection_t;

namespace NLSWIN {
class NLSWIN_API_PRIVATE WindowX11 : public Window {
   private:
   xcb_connection_t *m_xServerConnection = nullptr;

   public:
   WindowX11();
   virtual int SetSomething(int value) override;
};
}  // namespace NLSWIN
