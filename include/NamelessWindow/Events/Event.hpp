#pragma once

#include <string>
#include <typeindex>
#include <variant>

#include "../NLSAPI.h"
#include "Key.hpp"

namespace NLSWIN {

using EventType = std::type_index;
using WindowID = uint32_t;

struct NLSWIN_API_PUBLIC KeyEvent {
   static EventType type;
   std::string_view keyName;
   KeyCode code;
   KeyPressType pressType;
   WindowID sourceWindow;
};

struct NLSWIN_API_PUBLIC MouseButtonEvent {
   ButtonValue button;
   ButtonPressType type;
   float xPos;
   float yPos;
};

struct NLSWIN_API_PUBLIC MouseScrollEvent {
   ScrollType scrollType;
   float xPos;
   float yPos;
};

struct NLSWIN_API_PUBLIC MouseMovementEvent {
   float newXPos;
   float newYPos;
};

struct NLSWIN_API_PUBLIC MouseDeltaMovementEvent {
   float deltaX;
   float deltaY;
};

struct NLSWIN_API_PUBLIC MouseRawDeltaMovementEvent {
   float deltaX;
   float deltaY;
};

struct NLSWIN_API_PUBLIC MouseEnterEvent {
   float xPos;
   float yPos;
};

// Currently for internal use only.
struct NLSWIN_API_PUBLIC WindowCloseEvent {
   static EventType type;
};

struct NLSWIN_API_PUBLIC WindowFocusedEvent {
   static EventType type;
};

struct NLSWIN_API_PUBLIC WindowResizeEvent {
   int newWidth;
   int newHeight;
};

using Event = std::variant<KeyEvent, WindowCloseEvent, WindowFocusedEvent, WindowResizeEvent,
                           MouseButtonEvent, MouseScrollEvent, MouseMovementEvent, MouseDeltaMovementEvent,
                           MouseEnterEvent, MouseRawDeltaMovementEvent>;

}  // namespace NLSWIN