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
#define explicit _explicit
#include <xcb/xkb.h>
#undef explicit

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
   X11Keyboard() = default;
   X11Keyboard(KeyboardDeviceInfo info);

   private:
   void ProcessGenericEvent(xcb_generic_event_t *event) override;

   [[nodiscard]] Event ProcessKeyEvent(xcb_ge_generic_event_t *event);
   [[nodiscard]] xkb_keysym_t GetSymFromKeyCode(unsigned int keycode);
   void UpdateLockedModifiers(xcb_xkb_state_notify_event_t *stateNotify);
   void UpdateDepressedModifiers(NLSWIN::KeyValue val, bool pressed);
   std::array<bool, 512> m_InternalKeyState;
   xkb_context *m_keyboardContext {nullptr};
   xkb_keymap *m_keymap;

   // We use two seperate state objects for the keyboard.
   // DummyState: This state always has most modifiers always disabled, regardless of the real state of the
   // keyboard. The only modifier that it reflects correctly is NumLock. DummyState is used when determining
   // KeyValues. This means if the end-user wants to explicitly check for things like !, capital letters, etc,
   // when handling key events, they must do so themselves with the KeyEvent modifiers.
   //
   // RealState: Tracks the actual modifier state of the keyboard. This state will properly reflect Sym
   // Transformations beyond just the NumLock key. It's primarily used for setting the modifier state in
   // KeyEvents and in determining CharacterEvent values.
   xkb_state *m_dummyState {nullptr};
   xkb_state *m_realState {nullptr};
   KeyModifiers m_Mods {false};

   const xcb_input_xi_event_mask_t m_inputEventMask {
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_KEY_PRESS | XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE)};

   std::unordered_map<unsigned int, NLSWIN::KeyValue> m_keyTranslationTable = {
      {XKB_KEY_0, NLSWIN::KeyValue::KEY_0},
      {XKB_KEY_1, NLSWIN::KeyValue::KEY_1},
      {XKB_KEY_2, NLSWIN::KeyValue::KEY_2},
      {XKB_KEY_3, NLSWIN::KeyValue::KEY_3},
      {XKB_KEY_4, NLSWIN::KeyValue::KEY_4},
      {XKB_KEY_5, NLSWIN::KeyValue::KEY_5},
      {XKB_KEY_6, NLSWIN::KeyValue::KEY_6},
      {XKB_KEY_7, NLSWIN::KeyValue::KEY_7},
      {XKB_KEY_8, NLSWIN::KeyValue::KEY_8},
      {XKB_KEY_9, NLSWIN::KeyValue::KEY_9},
      {XKB_KEY_a, NLSWIN::KeyValue::KEY_A},
      {XKB_KEY_b, NLSWIN::KeyValue::KEY_B},
      {XKB_KEY_c, NLSWIN::KeyValue::KEY_C},
      {XKB_KEY_d, NLSWIN::KeyValue::KEY_D},
      {XKB_KEY_e, NLSWIN::KeyValue::KEY_E},
      {XKB_KEY_f, NLSWIN::KeyValue::KEY_F},
      {XKB_KEY_g, NLSWIN::KeyValue::KEY_G},
      {XKB_KEY_h, NLSWIN::KeyValue::KEY_H},
      {XKB_KEY_i, NLSWIN::KeyValue::KEY_I},
      {XKB_KEY_j, NLSWIN::KeyValue::KEY_J},
      {XKB_KEY_k, NLSWIN::KeyValue::KEY_K},
      {XKB_KEY_l, NLSWIN::KeyValue::KEY_L},
      {XKB_KEY_m, NLSWIN::KeyValue::KEY_M},
      {XKB_KEY_n, NLSWIN::KeyValue::KEY_N},
      {XKB_KEY_o, NLSWIN::KeyValue::KEY_O},
      {XKB_KEY_p, NLSWIN::KeyValue::KEY_P},
      {XKB_KEY_q, NLSWIN::KeyValue::KEY_Q},
      {XKB_KEY_r, NLSWIN::KeyValue::KEY_R},
      {XKB_KEY_s, NLSWIN::KeyValue::KEY_S},
      {XKB_KEY_t, NLSWIN::KeyValue::KEY_T},
      {XKB_KEY_u, NLSWIN::KeyValue::KEY_U},
      {XKB_KEY_v, NLSWIN::KeyValue::KEY_V},
      {XKB_KEY_w, NLSWIN::KeyValue::KEY_W},
      {XKB_KEY_x, NLSWIN::KeyValue::KEY_X},
      {XKB_KEY_y, NLSWIN::KeyValue::KEY_Y},
      {XKB_KEY_z, NLSWIN::KeyValue::KEY_Z},
      {XKB_KEY_A, NLSWIN::KeyValue::KEY_A},
      {XKB_KEY_B, NLSWIN::KeyValue::KEY_B},
      {XKB_KEY_C, NLSWIN::KeyValue::KEY_C},
      {XKB_KEY_D, NLSWIN::KeyValue::KEY_D},
      {XKB_KEY_E, NLSWIN::KeyValue::KEY_E},
      {XKB_KEY_F, NLSWIN::KeyValue::KEY_F},
      {XKB_KEY_G, NLSWIN::KeyValue::KEY_G},
      {XKB_KEY_H, NLSWIN::KeyValue::KEY_H},
      {XKB_KEY_I, NLSWIN::KeyValue::KEY_I},
      {XKB_KEY_J, NLSWIN::KeyValue::KEY_J},
      {XKB_KEY_K, NLSWIN::KeyValue::KEY_K},
      {XKB_KEY_L, NLSWIN::KeyValue::KEY_L},
      {XKB_KEY_M, NLSWIN::KeyValue::KEY_M},
      {XKB_KEY_N, NLSWIN::KeyValue::KEY_N},
      {XKB_KEY_O, NLSWIN::KeyValue::KEY_O},
      {XKB_KEY_P, NLSWIN::KeyValue::KEY_P},
      {XKB_KEY_Q, NLSWIN::KeyValue::KEY_Q},
      {XKB_KEY_R, NLSWIN::KeyValue::KEY_R},
      {XKB_KEY_S, NLSWIN::KeyValue::KEY_S},
      {XKB_KEY_T, NLSWIN::KeyValue::KEY_T},
      {XKB_KEY_U, NLSWIN::KeyValue::KEY_U},
      {XKB_KEY_V, NLSWIN::KeyValue::KEY_V},
      {XKB_KEY_W, NLSWIN::KeyValue::KEY_W},
      {XKB_KEY_X, NLSWIN::KeyValue::KEY_X},
      {XKB_KEY_Y, NLSWIN::KeyValue::KEY_Y},
      {XKB_KEY_Z, NLSWIN::KeyValue::KEY_Z},
      {XKB_KEY_Escape, NLSWIN::KeyValue::KEY_ESC},
      {XKB_KEY_Shift_L, NLSWIN::KeyValue::KEY_LSHIFT},
      {XKB_KEY_Shift_R, NLSWIN::KeyValue::KEY_RSHIFT},
      {XKB_KEY_Control_L, NLSWIN::KeyValue::KEY_LCTRL},
      {XKB_KEY_Control_R, NLSWIN::KeyValue::KEY_RCTRL},
      {XKB_KEY_Super_L, NLSWIN::KeyValue::KEY_LSUPER},
      {XKB_KEY_Super_R, NLSWIN::KeyValue::KEY_RSUPER},
      {XKB_KEY_Alt_L, NLSWIN::KeyValue::KEY_LALT},
      {XKB_KEY_Alt_R, NLSWIN::KeyValue::KEY_RALT},
      {XKB_KEY_space, NLSWIN::KeyValue::KEY_SPACE},
      {XKB_KEY_Tab, NLSWIN::KeyValue::KEY_TAB},
      {XKB_KEY_comma, NLSWIN::KeyValue::KEY_COMMA},
      {XKB_KEY_period, NLSWIN::KeyValue::KEY_PERIOD},
      {XKB_KEY_slash, NLSWIN::KeyValue::KEY_FORWARDSLASH},
      {XKB_KEY_semicolon, NLSWIN::KeyValue::KEY_SEMICOLON},
      {XKB_KEY_apostrophe, NLSWIN::KeyValue::KEY_APOSTROPHE},
      {XKB_KEY_backslash, NLSWIN::KeyValue::KEY_BACKSLASH},
      {XKB_KEY_Return, NLSWIN::KeyValue::KEY_ENTER},
      {XKB_KEY_bracketleft, NLSWIN::KeyValue::KEY_LBRACKET},
      {XKB_KEY_bracketright, NLSWIN::KeyValue::KEY_RBRACKET},
      {XKB_KEY_minus, NLSWIN::KeyValue::KEY_DASH},
      {XKB_KEY_equal, NLSWIN::KeyValue::KEY_EQUALS},
      {XKB_KEY_BackSpace, NLSWIN::KeyValue::KEY_BACKSPACE},
      {XKB_KEY_grave, NLSWIN::KeyValue::KEY_TILDE},
      {XKB_KEY_F1, NLSWIN::KeyValue::KEY_F1},
      {XKB_KEY_F2, NLSWIN::KeyValue::KEY_F2},
      {XKB_KEY_F3, NLSWIN::KeyValue::KEY_F3},
      {XKB_KEY_F4, NLSWIN::KeyValue::KEY_F4},
      {XKB_KEY_F5, NLSWIN::KeyValue::KEY_F5},
      {XKB_KEY_F6, NLSWIN::KeyValue::KEY_F6},
      {XKB_KEY_F7, NLSWIN::KeyValue::KEY_F7},
      {XKB_KEY_F8, NLSWIN::KeyValue::KEY_F8},
      {XKB_KEY_F9, NLSWIN::KeyValue::KEY_F9},
      {XKB_KEY_F10, NLSWIN::KeyValue::KEY_F10},
      {XKB_KEY_F11, NLSWIN::KeyValue::KEY_F11},
      {XKB_KEY_F12, NLSWIN::KeyValue::KEY_F12},
      {XKB_KEY_Left, NLSWIN::KeyValue::KEY_LEFT},
      {XKB_KEY_Right, NLSWIN::KeyValue::KEY_RIGHT},
      {XKB_KEY_Up, NLSWIN::KeyValue::KEY_UP},
      {XKB_KEY_Down, NLSWIN::KeyValue::KEY_DOWN},
      {XKB_KEY_Caps_Lock, NLSWIN::KeyValue::KEY_CAPSLOCK},
      {XKB_KEY_Scroll_Lock, NLSWIN::KeyValue::KEY_SCROLL_LOCK},
      {XKB_KEY_Num_Lock, NLSWIN::KeyValue::KEY_NUMLOCK},
      {XKB_KEY_Print, NLSWIN::KeyValue::KEY_PRINTSCREEN},
      {XKB_KEY_Pause, NLSWIN::KeyValue::KEY_PAUSE},
      {XKB_KEY_Insert, NLSWIN::KeyValue::KEY_INSERT},
      {XKB_KEY_Home, NLSWIN::KeyValue::KEY_HOME},
      {XKB_KEY_Page_Up, NLSWIN::KeyValue::KEY_PAGEUP},
      {XKB_KEY_Page_Down, NLSWIN::KeyValue::KEY_PAGEDOWN},
      {XKB_KEY_Delete, NLSWIN::KeyValue::KEY_DELETE},
      {XKB_KEY_End, NLSWIN::KeyValue::KEY_END},
      {XKB_KEY_Clear, NLSWIN::KeyValue::KEY_CLEAR},
      {XKB_KEY_KP_0, NLSWIN::KeyValue::KEY_NUMPAD_0},
      {XKB_KEY_KP_1, NLSWIN::KeyValue::KEY_NUMPAD_1},
      {XKB_KEY_KP_2, NLSWIN::KeyValue::KEY_NUMPAD_2},
      {XKB_KEY_KP_3, NLSWIN::KeyValue::KEY_NUMPAD_3},
      {XKB_KEY_KP_4, NLSWIN::KeyValue::KEY_NUMPAD_4},
      {XKB_KEY_KP_5, NLSWIN::KeyValue::KEY_NUMPAD_5},
      {XKB_KEY_KP_6, NLSWIN::KeyValue::KEY_NUMPAD_6},
      {XKB_KEY_KP_7, NLSWIN::KeyValue::KEY_NUMPAD_7},
      {XKB_KEY_KP_8, NLSWIN::KeyValue::KEY_NUMPAD_8},
      {XKB_KEY_KP_9, NLSWIN::KeyValue::KEY_NUMPAD_9},
      {XKB_KEY_KP_Decimal, NLSWIN::KeyValue::KEY_NUMPAD_PERIOD},
      {XKB_KEY_KP_Divide, NLSWIN::KeyValue::KEY_NUMPAD_DIVIDE},
      {XKB_KEY_KP_Multiply, NLSWIN::KeyValue::KEY_NUMPAD_MULTIPLY},
      {XKB_KEY_KP_Subtract, NLSWIN::KeyValue::KEY_NUMPAD_SUBTRACT},
      {XKB_KEY_KP_Add, NLSWIN::KeyValue::KEY_NUMPAD_ADD},
      {XKB_KEY_KP_Enter, NLSWIN::KeyValue::KEY_NUMPAD_ENTER},
      {XKB_KEY_KP_Insert, NLSWIN::KeyValue::KEY_INSERT},
      {XKB_KEY_KP_Delete, NLSWIN::KeyValue::KEY_DELETE},
      {XKB_KEY_KP_End, NLSWIN::KeyValue::KEY_END},
      {XKB_KEY_KP_Down, NLSWIN::KeyValue::KEY_DOWN},
      {XKB_KEY_KP_Page_Down, NLSWIN::KeyValue::KEY_PAGEDOWN},
      {XKB_KEY_KP_Left, NLSWIN::KeyValue::KEY_LEFT},
      {XKB_KEY_KP_Right, NLSWIN::KeyValue::KEY_RIGHT},
      {XKB_KEY_KP_Home, NLSWIN::KeyValue::KEY_HOME},
      {XKB_KEY_KP_Up, NLSWIN::KeyValue::KEY_UP},
      {XKB_KEY_KP_Page_Up, NLSWIN::KeyValue::KEY_PAGEUP},
      {XKB_KEY_KP_Begin,
       NLSWIN::KeyValue::KEY_CLEAR}  // Weird case on linux, this key doesnt seem to be universally defined.
   };
};
}  // namespace NLSWIN