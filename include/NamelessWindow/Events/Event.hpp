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
/*! @headerfile "Events/Event.hpp"
/*! Each created window is assigned one of these unique WindowIDs. */
using WindowID = uint32_t;

/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp"
/*! Generated whenever the user interacts with the keyboard. */
struct NLSWIN_API_PUBLIC KeyEvent {
   std::string_view keyName; /*!< The human-readable name of the keyboard key that generated the event. */
   KeyCode code;             /*!< Keycode information for the event. @see KeyCode */
   KeyPressType pressType;   /*!< Whether this event was a press, release or repeat. */
   WindowID sourceWindow;    /*!< The ID of the window that this event came from. */
};

/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp"
/*! Generated whenever the user interacts with a pointer button. */
struct NLSWIN_API_PUBLIC MouseButtonEvent {
   ButtonValue button;    /*!< Which button was pressed. @see ButtonValue */
   ButtonPressType type;  /*!< Whether the event was a press or release. */
   float xPos;            /*!< The X pixel coordinate where the event was generated in the source window. */
   float yPos;            /*!< The Y pixel coordinate where the event was generated in the source window. */
   WindowID sourceWindow; /*!< The ID of the window that this event came from */
};

/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp"
/*! Generated whenever the user interacts with the scroll wheel on a pointer. */
struct NLSWIN_API_PUBLIC MouseScrollEvent {
   ScrollType scrollType; /*!< The direction the scroll wheel was scrolled in. @see ScrollType */
   float xPos;            /*!< The X pixel coordinate where the event was generated in the source window. */
   float yPos;            /*!< The Y pixel coordinate where the event was generated in the source window. */
   WindowID sourceWindow; /*!< The ID of the window that this event came from */
};

/*!
 * @brief Generated whenever the MasterPointer's cursor is moved to a new position inside a client window.
 * @see MasterPointer
 * @ingroup Common
 * @headerfile "Events/Event.hpp"
 */
struct NLSWIN_API_PUBLIC MouseMovementEvent {
   float newXPos;         /*!< The X pixel coordinate of the new location of the cursor. */
   float newYPos;         /*!< The Y pixel coordinate of the new location of the cursor. */
   WindowID sourceWindow; /*!< The ID of the window that this event came from */
};

/*!
 * @brief Generated whenever pointer movement occurs.
 *
 * Provides the delta movement of the pointer, after platform-specific mouse acceleration/smoothing has been
 * applied. If you would look raw input data that is not impacted by mouse acceleration, use
 * MouseRawDeltaMovementEvent instead.
 * @see MouseRawDeltaMovementEvent
 * @ingroup Common
 * @headerfile "Events/Event.hpp"
 */
struct NLSWIN_API_PUBLIC MouseDeltaMovementEvent {
   float deltaX;
   float deltaY;
};

/*!
 * @brief Generated whenever pointer movement occurs.
 *
 * Provides the raw pointer movement data, before platform-specific mouse acceleration/smoothing has been
 * applied.
 * @ingroup Common
 * @headerfile "Events/Event.hpp"
 */
struct NLSWIN_API_PUBLIC MouseRawDeltaMovementEvent {
   float deltaX;
   float deltaY;
};

/*!
 * @brief Generated whenever the MasterPointer cursor enters within the bounds of an application window.
 * @see MasterPointer
 * @ingroup Common
 * @headerfile "Events/Event.hpp"
 */
struct NLSWIN_API_PUBLIC MouseEnterEvent {
   float xPos;            /*!< The X coordinate where the cursor entered the window. */
   float yPos;            /*!< The Y coordinate where the cursor entered the window. */
   WindowID sourceWindow; /*!< The window the cursor entered. */
};

/*!
 * @brief Generated whenever the MasterPointer cursor leaves the bounds of an application window.
 * @see MasterPointer
 * @ingroup Common
 * @headerfile "Events/Event.hpp"
 */
struct NLSWIN_API_PUBLIC MouseLeaveEvent {
   float xPos;            /*!< The X coordinate where the cursor left the window. */
   float yPos;            /*!< The Y coordinate where the cursor left the window. */
   WindowID sourceWindow; /*!< The window the cursor left. */
};

/*! Generated whenever an application window receives focus. */
/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
struct NLSWIN_API_PUBLIC WindowFocusedEvent {};

/*! Generated whenever an application window is resized. */
/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
struct NLSWIN_API_PUBLIC WindowResizeEvent {
   int newWidth;  /*!< The new width of the application window. */
   int newHeight; /*!< The new height of the application window. */
};

/*! Generic NLSWIN Event. */
/*! @ingroup Common */
/*! @headerfile "Events/Event.hpp" */
using Event = std::variant<std::monostate, KeyEvent, WindowFocusedEvent, WindowResizeEvent, MouseButtonEvent,
                           MouseScrollEvent, MouseMovementEvent, MouseDeltaMovementEvent, MouseEnterEvent,
                           MouseLeaveEvent, MouseRawDeltaMovementEvent>;

}  // namespace NLSWIN