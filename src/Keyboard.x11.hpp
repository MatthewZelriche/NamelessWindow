#pragma once

#include <xcb/xcb.h>

#include "EventListener.x11.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/NLSAPI.h"

namespace NLSWIN {
class NLSWIN_API_PRIVATE Keyboard::KeyboardImpl : public EventListenerX11 {
   private:
   static xcb_connection_t *m_connection;

   public:
   KeyboardImpl();
};
}  // namespace NLSWIN