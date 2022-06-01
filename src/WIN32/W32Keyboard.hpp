#pragma once

#include <unordered_set>

#include "NamelessWindow/Keyboard.hpp"
#include "NamelessWindow/NLSAPI.hpp"
#include "W32InputDevice.hpp"

#define MAGIC_ENUM_RANGE_MIN -1
#define MAGIC_ENUM_RANGE_MAX 512
#include <MagicEnum/magic_enum.hpp>

namespace NLSWIN {

class NLSWIN_API_PRIVATE W32Keyboard : virtual public Keyboard, public W32InputDevice {
   public:
   W32Keyboard(KeyboardDeviceInfo device);

   private:
   void ProcessGenericEvent(MSG event) override;

   /**
    * @brief Process a Raw Win32API key event.
    *
    * @param event The Win32 event to process.
    * @param window The source window for this event.
    * @return Event A processed, platform-agnostic event.
    */
   KeyEvent ProcessKeyEvent(RAWKEYBOARD event, HWND window);
   /**
    * @brief Handle certain windows peculiarities regarding the virtual keycode.
    *
    * @param obfuscatedEvent The Win32 keyboard event to process.
    * @return USHORT The actual, final virtual keycode for this event.
    * @see https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
    */
   USHORT DeobfuscateWindowsVKey(RAWKEYBOARD obfuscatedEvent);

   /**
    * @brief Updates W32Keyboard state to reflect which toggle keys are toggled on.
    *
    * Toggled keys include KEY_CAPSLOCK, KEY_SCROLL_LOCK and KEY_NUMLOCK.
    */
   void UpdateToggleKeyStatus();

   /**
    * @brief Determines the modifiers currently active for this keyboard device.
    *
    * @return KeyModifiers Which modifiers are currently active at the time of this method call.
    */
   KeyModifiers ParseModifierState();
   void UpdateWin32KeyboardState(USHORT vKey, KeyValue value, KeyPressType type);
   std::array<bool, 512> m_InternalKeyState {false};
   std::array<uint8_t, 256> m_win32KeyboardState {false};
   uint64_t deviceSpecifier {0};
   HWND keyboardFocusedWindow {nullptr};
   bool capsLockOn {false};
   bool scrollLockOn {false};
   bool numLockOn {false};

   const std::unordered_set<KeyValue> m_lockMods = {KeyValue::KEY_CAPSLOCK, KeyValue::KEY_NUMLOCK,
                                                    KeyValue::KEY_SCROLL_LOCK};
   std::unordered_map<unsigned int, KeyValue> m_translationTable = {
      {0x30, KeyValue::KEY_0},
      {0x31, KeyValue::KEY_1},
      {0x32, KeyValue::KEY_2},
      {0x33, KeyValue::KEY_3},
      {0x34, KeyValue::KEY_4},
      {0x35, KeyValue::KEY_5},
      {0x36, KeyValue::KEY_6},
      {0x37, KeyValue::KEY_7},
      {0x38, KeyValue::KEY_8},
      {0x39, KeyValue::KEY_9},
      {0x41, KeyValue::KEY_A},
      {0x42, KeyValue::KEY_B},
      {0x43, KeyValue::KEY_C},
      {0x44, KeyValue::KEY_D},
      {0x45, KeyValue::KEY_E},
      {0x46, KeyValue::KEY_F},
      {0x47, KeyValue::KEY_G},
      {0x48, KeyValue::KEY_H},
      {0x49, KeyValue::KEY_I},
      {0x4A, KeyValue::KEY_J},
      {0x4B, KeyValue::KEY_K},
      {0x4C, KeyValue::KEY_L},
      {0x4D, KeyValue::KEY_M},
      {0x4E, KeyValue::KEY_N},
      {0x4F, KeyValue::KEY_O},
      {0x50, KeyValue::KEY_P},
      {0x51, KeyValue::KEY_Q},
      {0x52, KeyValue::KEY_R},
      {0x53, KeyValue::KEY_S},
      {0x54, KeyValue::KEY_T},
      {0x55, KeyValue::KEY_U},
      {0x56, KeyValue::KEY_V},
      {0x57, KeyValue::KEY_W},
      {0x58, KeyValue::KEY_X},
      {0x59, KeyValue::KEY_Y},
      {0x5A, KeyValue::KEY_Z},
      {VK_ESCAPE, KeyValue::KEY_ESC},
      {VK_LSHIFT, KeyValue::KEY_LSHIFT},
      {VK_RSHIFT, KeyValue::KEY_RSHIFT},
      {VK_LCONTROL, KeyValue::KEY_LCTRL},
      {VK_RCONTROL, KeyValue::KEY_RCTRL},
      {VK_LWIN, KeyValue::KEY_LSUPER},
      {VK_RWIN, KeyValue::KEY_RSUPER},
      {VK_LMENU, KeyValue::KEY_LALT},
      {VK_RMENU, KeyValue::KEY_RALT},
      {VK_SPACE, KeyValue::KEY_SPACE},
      {VK_TAB, KeyValue::KEY_TAB},
      {VK_OEM_COMMA, KeyValue::KEY_COMMA},
      {VK_OEM_PERIOD, KeyValue::KEY_PERIOD},
      {VK_OEM_2, KeyValue::KEY_FORWARDSLASH},
      {VK_OEM_1, KeyValue::KEY_SEMICOLON},
      {VK_OEM_7, KeyValue::KEY_APOSTROPHE},
      {VK_OEM_5, KeyValue::KEY_BACKSLASH},
      {VK_RETURN, KeyValue::KEY_ENTER},
      {VK_OEM_4, KeyValue::KEY_LBRACKET},
      {VK_OEM_6, KeyValue::KEY_RBRACKET},
      {VK_OEM_MINUS, KeyValue::KEY_DASH},
      {VK_OEM_PLUS, KeyValue::KEY_EQUALS},
      {VK_BACK, KeyValue::KEY_BACKSPACE},
      {VK_OEM_3, KeyValue::KEY_TILDE},
      {VK_F1, KeyValue::KEY_F1},
      {VK_F2, KeyValue::KEY_F2},
      {VK_F3, KeyValue::KEY_F3},
      {VK_F4, KeyValue::KEY_F4},
      {VK_F5, KeyValue::KEY_F5},
      {VK_F6, KeyValue::KEY_F6},
      {VK_F7, KeyValue::KEY_F7},
      {VK_F8, KeyValue::KEY_F8},
      {VK_F9, KeyValue::KEY_F9},
      {VK_F10, KeyValue::KEY_F10},
      {VK_F11, KeyValue::KEY_F11},
      {VK_F12, KeyValue::KEY_F12},
      {VK_LEFT, KeyValue::KEY_LEFT},
      {VK_RIGHT, KeyValue::KEY_RIGHT},
      {VK_UP, KeyValue::KEY_UP},
      {VK_DOWN, KeyValue::KEY_DOWN},
      {VK_CAPITAL, KeyValue::KEY_CAPSLOCK},
      {VK_SCROLL, KeyValue::KEY_SCROLL_LOCK},
      {VK_NUMLOCK, KeyValue::KEY_NUMLOCK},
      {VK_SNAPSHOT, KeyValue::KEY_PRINTSCREEN},
      {VK_PAUSE, KeyValue::KEY_PAUSE},
      {VK_INSERT, KeyValue::KEY_INSERT},
      {VK_HOME, KeyValue::KEY_HOME},
      {VK_PRIOR, KeyValue::KEY_PAGEUP},
      {VK_NEXT, KeyValue::KEY_PAGEDOWN},
      {VK_DELETE, KeyValue::KEY_DELETE},
      {VK_END, KeyValue::KEY_END},
      {VK_CLEAR, KeyValue::KEY_CLEAR},
      {VK_NUMPAD0, KeyValue::KEY_NUMPAD_0},
      {VK_NUMPAD1, KeyValue::KEY_NUMPAD_1},
      {VK_NUMPAD2, KeyValue::KEY_NUMPAD_2},
      {VK_NUMPAD3, KeyValue::KEY_NUMPAD_3},
      {VK_NUMPAD4, KeyValue::KEY_NUMPAD_4},
      {VK_NUMPAD5, KeyValue::KEY_NUMPAD_5},
      {VK_NUMPAD6, KeyValue::KEY_NUMPAD_6},
      {VK_NUMPAD7, KeyValue::KEY_NUMPAD_7},
      {VK_NUMPAD8, KeyValue::KEY_NUMPAD_8},
      {VK_NUMPAD9, KeyValue::KEY_NUMPAD_9},
      {VK_DECIMAL, KeyValue::KEY_NUMPAD_PERIOD},
      {VK_DIVIDE, KeyValue::KEY_NUMPAD_DIVIDE},
      {VK_MULTIPLY, KeyValue::KEY_NUMPAD_MULTIPLY},
      {VK_SUBTRACT, KeyValue::KEY_NUMPAD_SUBTRACT},
      {VK_ADD, KeyValue::KEY_NUMPAD_ADD},
      {0x100, KeyValue::KEY_NUMPAD_ENTER}};
};

}  // namespace NLSWIN