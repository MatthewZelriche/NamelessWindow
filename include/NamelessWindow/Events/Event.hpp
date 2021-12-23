#pragma once

#include <string>
#include <typeindex>
#include <variant>

#include "../NLSAPI.h"
#include "Key.hpp"

namespace NLSWIN {

using EventType = std::type_index;

struct NLSWIN_API_PUBLIC KeyEvent {
   static EventType type;
   std::string_view keyName;
   KeyCode code;
   KeyPressType pressType;
};

// Currently for internal use only.
struct NLSWIN_API_PUBLIC WindowCloseEvent {
   static EventType type;
};

struct NLSWIN_API_PUBLIC WindowFocusedEvent {
   static EventType type;
};

using Event = std::variant<KeyEvent, WindowCloseEvent, WindowFocusedEvent>;

}  // namespace NLSWIN