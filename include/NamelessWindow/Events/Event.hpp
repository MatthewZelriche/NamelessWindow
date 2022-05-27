/*!
 * @file Event.hpp
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup Common Public API
 * @brief Documentation for public API that clients directly interact with.
 */
#pragma once

#include <string>
#include <variant>

#include "../NLSAPI.hpp"
#include "Key.hpp"

namespace NLSWIN {

/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
/*! Each created window is assigned one of these unique WindowIDs. */
using WindowID = uint32_t;

/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
/*! Generated whenever the user interacts with the keyboard. */
struct NLSWIN_API_PUBLIC KeyEvent {
   std::string keyName {"NULL"}; /*!< The human-readable name of the keyboard key that generated the event. */
   KeyCode code;                 /*!< Keycode information for the event. @see KeyCode */
   KeyPressType pressType {KeyPressType::UNKNOWN}; /*!< Whether this event was a press, release or repeat. */
   WindowID sourceWindow {0};                      /*!< The ID of the window that this event came from. */
};

/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
/*! Generated whenever the user interacts with a mouse button.
    Does not include button events on window border/title.*/
struct NLSWIN_API_PUBLIC MouseButtonEvent {
   ButtonValue button;    /*!< Which button was pressed. @see ButtonValue */
   ButtonPressType type;  /*!< Whether the event was a press or release. */
   float xPos;            /*!< The X pixel coordinate where the event was generated in the source window. */
   float yPos;            /*!< The Y pixel coordinate where the event was generated in the source window. */
   WindowID sourceWindow; /*!< The ID of the window that this event came from */
};

/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
/*! Generated whenever the user interacts with a raw mouse button. */
struct NLSWIN_API_PUBLIC RawMouseButtonEvent {
   ButtonValue button;   /*!< Which button was pressed. @see ButtonValue */
   ButtonPressType type; /*!< Whether the event was a press or release. */
};

/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
/*! Generated whenever the user interacts with the scroll wheel on a mouse within a user-created window.
    Does not include scrolls on window border/title. */
struct NLSWIN_API_PUBLIC MouseScrollEvent {
   ScrollType scrollType; /*!< The direction the scroll wheel was scrolled in. @see ScrollType */
   float xPos;            /*!< The X pixel coordinate where the event was generated in the source window. */
   float yPos;            /*!< The Y pixel coordinate where the event was generated in the source window. */
   WindowID sourceWindow; /*!< The ID of the window that this event came from */
};

/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
/*! Generated whenever the user interacts with the scroll wheel on a raw mouse. */
struct NLSWIN_API_PUBLIC RawMouseScrollEvent {
   ScrollType scrollType; /*!< The direction the scroll wheel was scrolled in. @see ScrollType */
};

/*!
 * @brief Generated whenever a cursor is moved to a new position inside a client window.
 *
 * Does not include button events on window border/title.
 * @todo Should window need to be focused to get this event?
 * @see Cursor
 * @ingroup Common
 * @headerfile "Events/Event.hpp"
 */
struct NLSWIN_API_PUBLIC MouseMovementEvent {
   float newXPos;         /*!< The X pixel coordinate of the new location of the cursor. */
   float newYPos;         /*!< The Y pixel coordinate of the new location of the cursor. */
   WindowID sourceWindow; /*!< The ID of the window that this event came from */
};

/*!
 * @brief Generated whenever mouse movement occurs.
 *
 * Provides the raw mouse movement data, before platform-specific mouse acceleration/smoothing has been
 * applied.
 * @ingroup Common
 * @headerfile "Events/Event.hpp"
 */
struct NLSWIN_API_PUBLIC RawMouseDeltaMovementEvent {
   float deltaX;
   float deltaY;
};

/*!
 * @brief Generated whenever a cursor enters within the bounds of an application window's client area.
 * @see Cursor
 * @ingroup Common
 * @headerfile "Events/Event.hpp"
 */
struct NLSWIN_API_PUBLIC MouseEnterEvent {
   float xPos;            /*!< The X coordinate where the cursor entered the window. */
   float yPos;            /*!< The Y coordinate where the cursor entered the window. */
   WindowID sourceWindow; /*!< The window the cursor entered. */
};

/*!
 * @brief Generated whenever a cursor leaves the bounds of an application window's client area.
 * @see Cursor
 * @ingroup Common
 * @headerfile "Events/Event.hpp"
 */
struct NLSWIN_API_PUBLIC MouseLeaveEvent {
   WindowID sourceWindow; /*!< The window the cursor left. */
};

/*! Generated whenever an application window receives focus. */
/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
struct NLSWIN_API_PUBLIC WindowFocusedEvent {
   WindowID sourceWindow; /*!< The window that was focused. */
};

/*! Generated whenever an application window is resized. */
/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
struct NLSWIN_API_PUBLIC WindowResizeEvent {
   int newWidth;          /*!< The new width of the application window. */
   int newHeight;         /*!< The new height of the application window. */
   WindowID sourceWindow; /*!< The window that was resized. */
};

struct NLSWIN_API_PUBLIC WindowRepositionEvent {
   int newX;              /*!< The new width of the application window. */
   int newY;              /*!< The new height of the application window. */
   WindowID sourceWindow; /*!< The window that was resized. */
};

struct NLSWIN_API_PUBLIC CharacterEvent {
   char character;        /*!< The ASCII character that was pressed on the keyboard. */
   WindowID sourceWindow; /*!< The window that received this character. */
};

/*! Generic NLSWIN Event. */
/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
using Event =
   std::variant<std::monostate, KeyEvent, WindowFocusedEvent, WindowResizeEvent, MouseButtonEvent,
                RawMouseButtonEvent, MouseScrollEvent, RawMouseScrollEvent, MouseMovementEvent,
                           MouseEnterEvent, MouseLeaveEvent, RawMouseDeltaMovementEvent,
                           WindowRepositionEvent, CharacterEvent>;

}  // namespace NLSWIN