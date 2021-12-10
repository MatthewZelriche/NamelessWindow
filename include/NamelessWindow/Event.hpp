#pragma once

#include <typeindex>
#include <variant>

#include "Key.hpp"
#include "NLSAPI.h"

namespace NLSWIN {

struct NLSWIN_API_PUBLIC KeyEvent {
   static std::type_index type;
   KeyCode code;
   KeyPressType pressType;
};

// Currently for internal use only.
struct NLSWIN_API_PUBLIC WindowCloseEvent {
   static std::type_index type;
};

struct NLSWIN_API_PUBLIC WindowFocusedEvent {
   static std::type_index type;
};

using Event     = std::variant<KeyEvent, WindowCloseEvent, WindowFocusedEvent>;
using EventType = std::type_index;

}  // namespace NLSWIN