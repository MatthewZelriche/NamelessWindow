#pragma once

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
   Event ProcessKeyEvent(RAWKEYBOARD event, HWND window);
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
   std::array<bool, 512> m_InternalKeyState {false};
   uint64_t deviceSpecifier {0};
   HWND keyboardFocusedWindow {nullptr};
   bool capsLockOn {false};
   bool scrollLockOn {false};
   bool numLockOn {false};

   std::unordered_map<unsigned int, KeyValue> m_translationTable = {{0x30, KEY_0},
                                                                    {0x31, KEY_1},
                                                                    {0x32, KEY_2},
                                                                    {0x33, KEY_3},
                                                                    {0x34, KEY_4},
                                                                    {0x35, KEY_5},
                                                                    {0x36, KEY_6},
                                                                    {0x37, KEY_7},
                                                                    {0x38, KEY_8},
                                                                    {0x39, KEY_9},
                                                                    {0x41, KEY_A},
                                                                    {0x42, KEY_B},
                                                                    {0x43, KEY_C},
                                                                    {0x44, KEY_D},
                                                                    {0x45, KEY_E},
                                                                    {0x46, KEY_F},
                                                                    {0x47, KEY_G},
                                                                    {0x48, KEY_H},
                                                                    {0x49, KEY_I},
                                                                    {0x4A, KEY_J},
                                                                    {0x4B, KEY_K},
                                                                    {0x4C, KEY_L},
                                                                    {0x4D, KEY_M},
                                                                    {0x4E, KEY_N},
                                                                    {0x4F, KEY_O},
                                                                    {0x50, KEY_P},
                                                                    {0x51, KEY_Q},
                                                                    {0x52, KEY_R},
                                                                    {0x53, KEY_S},
                                                                    {0x54, KEY_T},
                                                                    {0x55, KEY_U},
                                                                    {0x56, KEY_V},
                                                                    {0x57, KEY_W},
                                                                    {0x58, KEY_X},
                                                                    {0x59, KEY_Y},
                                                                    {0x5A, KEY_Z},
                                                                    {VK_ESCAPE, KEY_ESC},
                                                                    {VK_LSHIFT, KEY_LSHIFT},
                                                                    {VK_RSHIFT, KEY_RSHIFT},
                                                                    {VK_LCONTROL, KEY_LCTRL},
                                                                    {VK_RCONTROL, KEY_RCTRL},
                                                                    {VK_LWIN, KEY_LSUPER},
                                                                    {VK_RWIN, KEY_RSUPER},
                                                                    {VK_LMENU, KEY_LALT},
                                                                    {VK_RMENU, KEY_RALT},
                                                                    {VK_SPACE, KEY_SPACE},
                                                                    {VK_TAB, KEY_TAB},
                                                                    {VK_OEM_COMMA, KEY_COMMA},
                                                                    {VK_OEM_PERIOD, KEY_PERIOD},
                                                                    {VK_OEM_2, KEY_FORWARDSLASH},
                                                                    {VK_OEM_1, KEY_SEMICOLON},
                                                                    {VK_OEM_7, KEY_APOSTROPHE},
                                                                    {VK_OEM_5, KEY_BACKSLASH},
                                                                    {VK_RETURN, KEY_ENTER},
                                                                    {VK_OEM_4, KEY_LBRACKET},
                                                                    {VK_OEM_6, KEY_RBRACKET},
                                                                    {VK_OEM_MINUS, KEY_DASH},
                                                                    {VK_OEM_PLUS, KEY_EQUALS},
                                                                    {VK_BACK, KEY_BACKSPACE},
                                                                    {VK_OEM_3, KEY_TILDE},
                                                                    {VK_F1, KEY_F1},
                                                                    {VK_F2, KEY_F2},
                                                                    {VK_F3, KEY_F3},
                                                                    {VK_F4, KEY_F4},
                                                                    {VK_F5, KEY_F5},
                                                                    {VK_F6, KEY_F6},
                                                                    {VK_F7, KEY_F7},
                                                                    {VK_F8, KEY_F8},
                                                                    {VK_F9, KEY_F9},
                                                                    {VK_F10, KEY_F10},
                                                                    {VK_F11, KEY_F11},
                                                                    {VK_F12, KEY_F12},
                                                                    {VK_LEFT, KEY_LEFT},
                                                                    {VK_RIGHT, KEY_RIGHT},
                                                                    {VK_UP, KEY_UP},
                                                                    {VK_DOWN, KEY_DOWN},
                                                                    {VK_CAPITAL, KEY_CAPSLOCK},
                                                                    {VK_SCROLL, KEY_SCROLL_LOCK},
                                                                    {VK_NUMLOCK, KEY_NUMLOCK},
                                                                    {VK_SNAPSHOT, KEY_PRINTSCREEN},
                                                                    {VK_PAUSE, KEY_PAUSE},
                                                                    {VK_INSERT, KEY_INSERT},
                                                                    {VK_HOME, KEY_HOME},
                                                                    {VK_PRIOR, KEY_PAGEUP},
                                                                    {VK_NEXT, KEY_PAGEDOWN},
                                                                    {VK_DELETE, KEY_DELETE},
                                                                    {VK_END, KEY_END},
                                                                    {VK_CLEAR, KEY_CLEAR},
                                                                    {VK_NUMPAD0, KEY_NUMPAD_0},
                                                                    {VK_NUMPAD1, KEY_NUMPAD_1},
                                                                    {VK_NUMPAD2, KEY_NUMPAD_2},
                                                                    {VK_NUMPAD3, KEY_NUMPAD_3},
                                                                    {VK_NUMPAD4, KEY_NUMPAD_4},
                                                                    {VK_NUMPAD5, KEY_NUMPAD_5},
                                                                    {VK_NUMPAD6, KEY_NUMPAD_6},
                                                                    {VK_NUMPAD7, KEY_NUMPAD_7},
                                                                    {VK_NUMPAD8, KEY_NUMPAD_8},
                                                                    {VK_NUMPAD9, KEY_NUMPAD_9},
                                                                    {VK_DECIMAL, KEY_NUMPAD_PERIOD},
                                                                    {VK_DIVIDE, KEY_NUMPAD_DIVIDE},
                                                                    {VK_MULTIPLY, KEY_NUMPAD_MULTIPLY},
                                                                    {VK_SUBTRACT, KEY_NUMPAD_SUBTRACT},
                                                                    {VK_ADD, KEY_NUMPAD_ADD},
                                                                    {0x100, KEY_NUMPAD_ENTER}};
};

}  // namespace NLSWIN