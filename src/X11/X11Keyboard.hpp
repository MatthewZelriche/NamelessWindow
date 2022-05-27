/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup X11 Linux X11 API
 * @brief Platform-specific X11 implementation of the API
 */
#pragma once

#include <xcb/xinput.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>

#include <unordered_map>

#include "NamelessWindow/Events/Key.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "X11InputDevice.hpp"

namespace NLSWIN {
/**
 * @brief An instance of a deviceID corresponding to a Keyboard.
 * @ingroup X11
 */
class NLSWIN_API_PRIVATE X11Keyboard : public X11InputDevice, public Keyboard {
   public:
   X11Keyboard();
   X11Keyboard(KeyboardDeviceInfo info);

   private:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;

   [[nodiscard]] Event ProcessKeyEvent(xcb_ge_generic_event_t *event);
   [[nodiscard]] xkb_keysym_t GetSymFromKeyCode(unsigned int keycode);
   [[nodiscard]] KeyModifiers ParseModifierState(uint32_t mods, NLSWIN::KeyValue value, bool pressed);
   std::array<bool, 512> m_InternalKeyState;
   xkb_context *m_keyboardContext {nullptr};
   xkb_state *m_KeyboardState {nullptr};
   xkb_keymap *m_keymap;

   bool m_ShiftModifier {false};
   bool m_AltModifier {false};
   bool m_SuperModifier {false};
   bool m_CtrlModifier {false};

   const xcb_input_xi_event_mask_t m_inputEventMask {
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_KEY_PRESS | XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE)};

   std::unordered_map<unsigned int, int> m_keyTranslationTable = {
      {XKB_KEY_0, KEY_0},
      {XKB_KEY_1, KEY_1},
      {XKB_KEY_2, KEY_2},
      {XKB_KEY_3, KEY_3},
      {XKB_KEY_4, KEY_4},
      {XKB_KEY_5, KEY_5},
      {XKB_KEY_6, KEY_6},
      {XKB_KEY_7, KEY_7},
      {XKB_KEY_8, KEY_8},
      {XKB_KEY_9, KEY_9},
      {XKB_KEY_a, KEY_A},
      {XKB_KEY_b, KEY_B},
      {XKB_KEY_c, KEY_C},
      {XKB_KEY_d, KEY_D},
      {XKB_KEY_e, KEY_E},
      {XKB_KEY_f, KEY_F},
      {XKB_KEY_g, KEY_G},
      {XKB_KEY_h, KEY_H},
      {XKB_KEY_i, KEY_I},
      {XKB_KEY_j, KEY_J},
      {XKB_KEY_k, KEY_K},
      {XKB_KEY_l, KEY_L},
      {XKB_KEY_m, KEY_M},
      {XKB_KEY_n, KEY_N},
      {XKB_KEY_o, KEY_O},
      {XKB_KEY_p, KEY_P},
      {XKB_KEY_q, KEY_Q},
      {XKB_KEY_r, KEY_R},
      {XKB_KEY_s, KEY_S},
      {XKB_KEY_t, KEY_T},
      {XKB_KEY_u, KEY_U},
      {XKB_KEY_v, KEY_V},
      {XKB_KEY_w, KEY_W},
      {XKB_KEY_x, KEY_X},
      {XKB_KEY_y, KEY_Y},
      {XKB_KEY_z, KEY_Z},
      {XKB_KEY_Escape, KEY_ESC},
      {XKB_KEY_Shift_L, KEY_LSHIFT},
      {XKB_KEY_Shift_R, KEY_RSHIFT},
      {XKB_KEY_Control_L, KEY_LCTRL},
      {XKB_KEY_Control_R, KEY_RCTRL},
      {XKB_KEY_Super_L, KEY_LSUPER},
      {XKB_KEY_Super_R, KEY_RSUPER},
      {XKB_KEY_Alt_L, KEY_LALT},
      {XKB_KEY_Alt_R, KEY_RALT},
      {XKB_KEY_space, KEY_SPACE},
      {XKB_KEY_Tab, KEY_TAB},
      {XKB_KEY_comma, KEY_COMMA},
      {XKB_KEY_period, KEY_PERIOD},
      {XKB_KEY_slash, KEY_FORWARDSLASH},
      {XKB_KEY_semicolon, KEY_SEMICOLON},
      {XKB_KEY_apostrophe, KEY_APOSTROPHE},
      {XKB_KEY_backslash, KEY_BACKSLASH},
      {XKB_KEY_Return, KEY_ENTER},
      {XKB_KEY_bracketleft, KEY_LBRACKET},
      {XKB_KEY_bracketright, KEY_RBRACKET},
      {XKB_KEY_minus, KEY_DASH},
      {XKB_KEY_equal, KEY_EQUALS},
      {XKB_KEY_BackSpace, KEY_BACKSPACE},
      {XKB_KEY_grave, KEY_TILDE},
      {XKB_KEY_F1, KEY_F1},
      {XKB_KEY_F2, KEY_F2},
      {XKB_KEY_F3, KEY_F3},
      {XKB_KEY_F4, KEY_F4},
      {XKB_KEY_F5, KEY_F5},
      {XKB_KEY_F6, KEY_F6},
      {XKB_KEY_F7, KEY_F7},
      {XKB_KEY_F8, KEY_F8},
      {XKB_KEY_F9, KEY_F9},
      {XKB_KEY_F10, KEY_F10},
      {XKB_KEY_F11, KEY_F11},
      {XKB_KEY_F12, KEY_F12},
      {XKB_KEY_Left, KEY_LEFT},
      {XKB_KEY_Right, KEY_RIGHT},
      {XKB_KEY_Up, KEY_UP},
      {XKB_KEY_Down, KEY_DOWN},
      {XKB_KEY_Caps_Lock, KEY_CAPSLOCK},
      {XKB_KEY_Scroll_Lock, KEY_SCROLL_LOCK},
      {XKB_KEY_Num_Lock, KEY_NUMLOCK},
      {XKB_KEY_Print, KEY_PRINTSCREEN},
      {XKB_KEY_Pause, KEY_PAUSE},
      {XKB_KEY_Insert, KEY_INSERT},
      {XKB_KEY_Home, KEY_HOME},
      {XKB_KEY_Page_Up, KEY_PAGEUP},
      {XKB_KEY_Page_Down, KEY_PAGEDOWN},
      {XKB_KEY_Delete, KEY_DELETE},
      {XKB_KEY_End, KEY_END},
      {XKB_KEY_Clear, KEY_CLEAR},
      {XKB_KEY_KP_0, KEY_NUMPAD_0},
      {XKB_KEY_KP_1, KEY_NUMPAD_1},
      {XKB_KEY_KP_2, KEY_NUMPAD_2},
      {XKB_KEY_KP_3, KEY_NUMPAD_3},
      {XKB_KEY_KP_4, KEY_NUMPAD_4},
      {XKB_KEY_KP_5, KEY_NUMPAD_5},
      {XKB_KEY_KP_6, KEY_NUMPAD_6},
      {XKB_KEY_KP_7, KEY_NUMPAD_7},
      {XKB_KEY_KP_8, KEY_NUMPAD_8},
      {XKB_KEY_KP_9, KEY_NUMPAD_9},
      {XKB_KEY_KP_Decimal, KEY_NUMPAD_PERIOD},
      {XKB_KEY_KP_Divide, KEY_NUMPAD_DIVIDE},
      {XKB_KEY_KP_Multiply, KEY_NUMPAD_MULTIPLY},
      {XKB_KEY_KP_Subtract, KEY_NUMPAD_SUBTRACT},
      {XKB_KEY_KP_Add, KEY_NUMPAD_ADD},
      {XKB_KEY_KP_Enter, KEY_NUMPAD_ENTER},
      // Linux uses different sym values for kp keys when numlock is off.
      // For now we map them to the same regular keys they are meant to represent.
      {XKB_KEY_KP_Insert, KEY_INSERT},
      {XKB_KEY_KP_Delete, KEY_DELETE},
      {XKB_KEY_KP_End, KEY_END},
      {XKB_KEY_KP_Down, KEY_DOWN},
      {XKB_KEY_KP_Page_Down, KEY_PAGEDOWN},
      {XKB_KEY_KP_Left, KEY_LEFT},
      {XKB_KEY_KP_Right, KEY_RIGHT},
      {XKB_KEY_KP_Home, KEY_HOME},
      {XKB_KEY_KP_Up, KEY_UP},
      {XKB_KEY_KP_Page_Up, KEY_PAGEUP},
      {XKB_KEY_KP_Begin, KEY_CLEAR}  // Weird case on linux, this key doesnt seem to be universally defined.
   };
};
}  // namespace NLSWIN