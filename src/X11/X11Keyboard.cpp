#include "X11Keyboard.hpp"

#include <X11/Xlib.h>
#include <xcb/xproto.h>
#include <xkbcommon/xkbcommon-compat.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon.h>

#include "MagicEnum/magic_enum.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/Key.hpp"
#include "X11EventBus.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::shared_ptr<Keyboard> Keyboard::Create() {
   std::shared_ptr<X11Keyboard> impl = std::make_shared<X11Keyboard>();
   X11EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

std::shared_ptr<Keyboard> Keyboard::Create(KeyboardDeviceInfo device) {
   std::shared_ptr<X11Keyboard> impl = std::make_shared<X11Keyboard>(device);
   X11EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

std::vector<KeyboardDeviceInfo> Keyboard::EnumerateKeyboards() noexcept {
   return EnumerateDevicesX11<KeyboardDeviceInfo>(XCB_INPUT_DEVICE_TYPE_SLAVE_KEYBOARD);
}

X11Keyboard::X11Keyboard() {
   m_keyboardContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
   m_deviceID = xkb_x11_get_core_keyboard_device_id(XConnection::GetConnection());
   auto deviceKeymap = xkb_x11_keymap_new_from_device(m_keyboardContext, XConnection::GetConnection(),
                                                      m_deviceID, XKB_KEYMAP_COMPILE_NO_FLAGS);
   m_KeyboardState = xkb_x11_state_new_from_device(deviceKeymap, XConnection::GetConnection(), m_deviceID);
   xkb_keymap_unref(deviceKeymap);
   SubscribeToWindowSpecificXInput2Events(m_inputEventMask);

   m_InternalKeyState.fill(false);
}

X11Keyboard::X11Keyboard(KeyboardDeviceInfo info) {
   m_keyboardContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
   m_deviceID = info.platformSpecificIdentifier;
   auto deviceKeymap = xkb_x11_keymap_new_from_device(m_keyboardContext, XConnection::GetConnection(),
                                                      m_deviceID, XKB_KEYMAP_COMPILE_NO_FLAGS);
   m_KeyboardState = xkb_x11_state_new_from_device(deviceKeymap, XConnection::GetConnection(), m_deviceID);
   xkb_keymap_unref(deviceKeymap);
   SubscribeToWindowSpecificXInput2Events(m_inputEventMask);
}

void X11Keyboard::ProcessGenericEvent(xcb_generic_event_t *event) {
   if ((event->response_type & ~0x80) != XCB_GE_GENERIC) {
      return;
   }
   xcb_ge_generic_event_t *genericEvent = reinterpret_cast<xcb_ge_generic_event_t *>(event);
   switch (genericEvent->event_type) {
      case XCB_INPUT_KEY_RELEASE:
      case XCB_INPUT_KEY_PRESS: {
         xcb_input_key_press_event_t *keyEvent =
            reinterpret_cast<xcb_input_key_press_event_t *>(genericEvent);
         if (GetSubscribedWindows().count(keyEvent->event)) {
            if (m_deviceID == keyEvent->deviceid || m_deviceID == XCB_INPUT_DEVICE_ALL_MASTER) {
               Event processedEvent = ProcessKeyEvent(genericEvent);
               PushEvent(processedEvent);
            }
         }
      }
   }
}

Event X11Keyboard::ProcessKeyEvent(xcb_ge_generic_event_t *event) {
   KeyEvent keyEvent;
   switch (event->event_type) {
      case XCB_INPUT_KEY_PRESS: {
         xcb_input_key_press_event_t *pressEvent = reinterpret_cast<xcb_input_key_press_event_t *>(event);
         xkb_keysym_t keySym = GetSymFromKeyCode(pressEvent->detail);
         if (m_keyTranslationTable.count(keySym)) {
            keyEvent.code.value = (KeyValue)m_keyTranslationTable[keySym];
         } else {
            // Return null key event.
            return KeyEvent();
         }
         keyEvent.code.modifiers =
            ParseModifierState(pressEvent->mods.effective, pressEvent->detail, keyEvent.code.value, true);

         keyEvent.keyName = magic_enum::enum_name(keyEvent.code.value);
         keyEvent.sourceWindow = GetSubscribedWindows().at(pressEvent->event).lock()->GetGenericID();
         if (m_InternalKeyState[pressEvent->detail] == true) {
            keyEvent.pressType = KeyPressType::REPEAT;
         } else {
            keyEvent.pressType = KeyPressType::PRESSED;
            m_InternalKeyState[pressEvent->detail] = true;
         }

         // Handle CharacterEvents last.
         char character = (char)xkb_state_key_get_utf32(m_KeyboardState, pressEvent->detail);
         if (isprint(character)) {
            PushEvent(NLSWIN::CharacterEvent {
               character, GetSubscribedWindows().at(pressEvent->event).lock()->GetGenericID()});
         }
         break;
      }
      case XCB_INPUT_KEY_RELEASE: {
         xcb_input_key_release_event_t *releaseEvent =
            reinterpret_cast<xcb_input_key_release_event_t *>(event);
         xkb_keysym_t keySym = GetSymFromKeyCode(releaseEvent->detail);
         if (m_keyTranslationTable.count(keySym)) {
            keyEvent.code.value = (KeyValue)m_keyTranslationTable[keySym];
         } else {
            // Return null key event.
            return KeyEvent();
         }
         keyEvent.code.modifiers = ParseModifierState(releaseEvent->mods.effective, releaseEvent->detail,
                                                      keyEvent.code.value, false);

         keyEvent.keyName = magic_enum::enum_name(keyEvent.code.value);
         keyEvent.pressType = KeyPressType::RELEASED;
         keyEvent.sourceWindow = GetSubscribedWindows().at(releaseEvent->event).lock()->GetGenericID();
         m_InternalKeyState[releaseEvent->detail] = false;
         break;
      }
   }
   return keyEvent;
}

KeyModifiers X11Keyboard::ParseModifierState(uint32_t mods, xkb_keycode_t code, NLSWIN::KeyValue value,
                                             bool pressed) {
   if (value == NLSWIN::KEY_LSHIFT || value == NLSWIN::KEY_RSHIFT) {
      m_Mods.shift = pressed;
      xkb_state_update_key(m_KeyboardState, code, m_Mods.shift ? XKB_KEY_DOWN : XKB_KEY_UP);
      std::cout << pressed << std::endl;
   } else if (value == NLSWIN::KEY_LALT || value == NLSWIN::KEY_RALT) {
      m_Mods.alt = pressed;
      xkb_state_update_key(m_KeyboardState, code, m_Mods.alt ? XKB_KEY_DOWN : XKB_KEY_UP);
   } else if (value == NLSWIN::KEY_LSUPER || value == NLSWIN::KEY_RSUPER) {
      m_Mods.super = pressed;
      xkb_state_update_key(m_KeyboardState, code, m_Mods.super ? XKB_KEY_DOWN : XKB_KEY_UP);
   } else if (value == NLSWIN::KEY_LCTRL || value == NLSWIN::KEY_RCTRL) {
      m_Mods.ctrl = pressed;
      xkb_state_update_key(m_KeyboardState, code, m_Mods.ctrl ? XKB_KEY_DOWN : XKB_KEY_UP);
   } else if (value == NLSWIN::KeyValue::KEY_CAPSLOCK) {
      m_Mods.capsLock = !m_Mods.capsLock;
      xkb_state_update_key(m_KeyboardState, code, m_Mods.capsLock ? XKB_KEY_DOWN : XKB_KEY_UP);
   } else if (value == NLSWIN::KeyValue::KEY_SCROLL_LOCK) {
      m_Mods.scrollLock = !m_Mods.scrollLock;
      xkb_state_update_key(m_KeyboardState, code, m_Mods.scrollLock ? XKB_KEY_DOWN : XKB_KEY_UP);
   } else if (value == NLSWIN::KeyValue::KEY_NUMLOCK) {
      m_Mods.numLock = !m_Mods.numLock;
      xkb_state_update_key(m_KeyboardState, code, m_Mods.numLock ? XKB_KEY_DOWN : XKB_KEY_UP);
   }

   return m_Mods;
}

xkb_keysym_t X11Keyboard::GetSymFromKeyCode(unsigned int keycode) {
   // TODO: Consider somehow converting this to xcb.
   const xkb_keysym_t *array;
   xkb_state_key_get_syms(m_KeyboardState, keycode, &array);
   return array[0];
}