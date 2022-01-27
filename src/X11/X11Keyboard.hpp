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
#include <xkbcommon/xkbcommon-x11.h>

#include <MagicEnum/magic_enum.hpp>

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
   void ProcessXInputEvent(xcb_ge_generic_event_t *event) override;

   [[nodiscard]] Event ProcessKeyEvent(xcb_ge_generic_event_t *event);
   [[nodiscard]] xkb_keysym_t GetSymFromKeyCode(unsigned int keycode);
   [[nodiscard]] KeyModifiers ParseModifierState(uint32_t mods);
   [[nodiscard]] KeyValue TranslateKey(xkb_keysym_t keysym);
   static constexpr std::size_t m_NumKeys = magic_enum::enum_count<KeyValue>();
   std::array<bool, m_NumKeys> m_InternalKeyState;
   xkb_context *m_keyboardContext {nullptr};
   xkb_state *m_KeyboardState {nullptr};

   const xcb_input_xi_event_mask_t m_inputEventMask {
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_KEY_PRESS | XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE)};

   std::unordered_map<xkb_keysym_t, KeyValue> m_keyTranslationTable = {
      {XKB_KEY_VoidSymbol, KeyValue::KEY_NULL},
      {XKB_KEY_0, KeyValue::KEY_0},
      {XKB_KEY_1, KeyValue::KEY_1},
      {XKB_KEY_2, KeyValue::KEY_2},
      {XKB_KEY_3, KeyValue::KEY_3},
      {XKB_KEY_4, KeyValue::KEY_4},
      {XKB_KEY_5, KeyValue::KEY_5},
      {XKB_KEY_6, KeyValue::KEY_6},
      {XKB_KEY_7, KeyValue::KEY_7},
      {XKB_KEY_8, KeyValue::KEY_8},
      {XKB_KEY_9, KeyValue::KEY_9},
      {XKB_KEY_a, KeyValue::KEY_A},
      {XKB_KEY_b, KeyValue::KEY_B},
      {XKB_KEY_c, KeyValue::KEY_C},
      {XKB_KEY_d, KeyValue::KEY_D},
      {XKB_KEY_e, KeyValue::KEY_E},
      {XKB_KEY_f, KeyValue::KEY_F},
      {XKB_KEY_g, KeyValue::KEY_G},
      {XKB_KEY_h, KeyValue::KEY_H},
      {XKB_KEY_i, KeyValue::KEY_I},
      {XKB_KEY_j, KeyValue::KEY_J},
      {XKB_KEY_k, KeyValue::KEY_K},
      {XKB_KEY_l, KeyValue::KEY_L},
      {XKB_KEY_m, KeyValue::KEY_M},
      {XKB_KEY_n, KeyValue::KEY_N},
      {XKB_KEY_o, KeyValue::KEY_O},
      {XKB_KEY_p, KeyValue::KEY_P},
      {XKB_KEY_q, KeyValue::KEY_Q},
      {XKB_KEY_r, KeyValue::KEY_R},
      {XKB_KEY_s, KeyValue::KEY_S},
      {XKB_KEY_t, KeyValue::KEY_T},
      {XKB_KEY_u, KeyValue::KEY_U},
      {XKB_KEY_v, KeyValue::KEY_V},
      {XKB_KEY_w, KeyValue::KEY_W},
      {XKB_KEY_x, KeyValue::KEY_X},
      {XKB_KEY_y, KeyValue::KEY_Y},
      {XKB_KEY_z, KeyValue::KEY_Z},
      {XKB_KEY_A, KeyValue::KEY_A},
      {XKB_KEY_B, KeyValue::KEY_B},
      {XKB_KEY_C, KeyValue::KEY_C},
      {XKB_KEY_D, KeyValue::KEY_D},
      {XKB_KEY_E, KeyValue::KEY_E},
      {XKB_KEY_F, KeyValue::KEY_F},
      {XKB_KEY_G, KeyValue::KEY_G},
      {XKB_KEY_H, KeyValue::KEY_H},
      {XKB_KEY_I, KeyValue::KEY_I},
      {XKB_KEY_J, KeyValue::KEY_J},
      {XKB_KEY_K, KeyValue::KEY_K},
      {XKB_KEY_L, KeyValue::KEY_L},
      {XKB_KEY_M, KeyValue::KEY_M},
      {XKB_KEY_N, KeyValue::KEY_N},
      {XKB_KEY_O, KeyValue::KEY_O},
      {XKB_KEY_P, KeyValue::KEY_P},
      {XKB_KEY_Q, KeyValue::KEY_Q},
      {XKB_KEY_R, KeyValue::KEY_R},
      {XKB_KEY_S, KeyValue::KEY_S},
      {XKB_KEY_T, KeyValue::KEY_T},
      {XKB_KEY_U, KeyValue::KEY_U},
      {XKB_KEY_V, KeyValue::KEY_V},
      {XKB_KEY_W, KeyValue::KEY_W},
      {XKB_KEY_X, KeyValue::KEY_X},
      {XKB_KEY_Y, KeyValue::KEY_Y},
      {XKB_KEY_Z, KeyValue::KEY_Z},
      {XKB_KEY_Escape, KeyValue::KEY_ESC},
      {XKB_KEY_Shift_L, KeyValue::KEY_LSHIFT},
      {XKB_KEY_Shift_R, KeyValue::KEY_RSHIFT},
      {XKB_KEY_Control_L, KeyValue::KEY_LCTRL},
      {XKB_KEY_Control_R, KeyValue::KEY_RCTRL},
      {XKB_KEY_Super_L, KeyValue::KEY_LSUPER},
      {XKB_KEY_Super_R, KeyValue::KEY_RSUPER},
      {XKB_KEY_Alt_L, KeyValue::KEY_LALT},
      {XKB_KEY_Alt_R, KeyValue::KEY_RALT},
      {XKB_KEY_space, KeyValue::KEY_SPACE},
      {XKB_KEY_comma, KeyValue::KEY_COMMA},
      {XKB_KEY_period, KeyValue::KEY_PERIOD},
      {XKB_KEY_slash, KeyValue::KEY_FORWARDSLASH},
      {XKB_KEY_semicolon, KeyValue::KEY_SEMICOLON},
      {XKB_KEY_apostrophe, KeyValue::KEY_APOSTROPHE},
      {XKB_KEY_backslash, KeyValue::KEY_BACKSLASH},
      {XKB_KEY_Return, KeyValue::KEY_ENTER},
      {XKB_KEY_bracketleft, KeyValue::KEY_LBRACKET},
      {XKB_KEY_bracketright, KeyValue::KEY_RBRACKET},
      {XKB_KEY_minus, KeyValue::KEY_DASH},
      {XKB_KEY_equal, KeyValue::KEY_EQUALS},
      {XKB_KEY_BackSpace, KeyValue::KEY_BACKSPACE},
      {XKB_KEY_grave, KeyValue::KEY_TILDE},
      {XKB_KEY_Caps_Lock, KeyValue::KEY_CAPSLOCK},
      {XKB_KEY_KP_Divide, KeyValue::KEY_NUMPAD_DIVIDE},
      {XKB_KEY_KP_Multiply, KeyValue::KEY_NUMPAD_MULTIPLY},
      {XKB_KEY_KP_Subtract, KeyValue::KEY_NUMPAD_SUBTRACT},
      {XKB_KEY_KP_Add, KeyValue::KEY_NUMPAD_ADD},
      {XKB_KEY_KP_Enter, KeyValue::KEY_NUMPAD_ENTER},
   };
};
}  // namespace NLSWIN