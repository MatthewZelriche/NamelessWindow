#include "X11Keyboard.hpp"

#include <X11/Xlib.h>
#include <xcb/xinput.h>
#include <xcb/xproto.h>
#include <xkbcommon/xkbcommon-compat.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon-names.h>
#include <xkbcommon/xkbcommon.h>

#include "MagicEnum/magic_enum.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Events/Key.hpp"
#include "NamelessWindow/Keyboard.hpp"
#include "X11EventBus.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::shared_ptr<Keyboard> Keyboard::Create() {
   xcb_input_device_id_t devID = xkb_x11_get_core_keyboard_device_id(XConnection::GetConnection());
   std::shared_ptr<X11Keyboard> impl = std::make_shared<X11Keyboard>(KeyboardDeviceInfo {"", devID});
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

X11Keyboard::X11Keyboard(KeyboardDeviceInfo info) {
   m_keyboardContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
   m_deviceID = info.platformSpecificIdentifier;
   m_keymap = xkb_x11_keymap_new_from_device(m_keyboardContext, XConnection::GetConnection(), m_deviceID,
                                             XKB_KEYMAP_COMPILE_NO_FLAGS);
   m_dummyState = xkb_state_new(m_keymap);
   m_realState = xkb_x11_state_new_from_device(m_keymap, XConnection::GetConnection(), m_deviceID);
   SubscribeToWindowSpecificXInput2Events(m_inputEventMask);

   // Subscribe for events that notify us when keyboard state has changed (modifiers).
   xcb_xkb_select_events(XConnection::GetConnection(), m_deviceID, XCB_XKB_EVENT_TYPE_STATE_NOTIFY, 0,
                         XCB_XKB_EVENT_TYPE_STATE_NOTIFY, XCB_XKB_MAP_PART_MODIFIER_MAP,
                         XCB_XKB_MAP_PART_MODIFIER_MAP, nullptr);
   m_InternalKeyState.fill(false);
}

void X11Keyboard::ProcessGenericEvent(xcb_generic_event_t *event) {
   // Handle locked modifiers (capslock, numlock, etc) first.
   if ((event->response_type & ~0x80) ==
       XConnection::GetXKBBaseEvent()) {  // Don't know what I need to do this, but it seems necessary.
      switch (event->pad0) {
         case XCB_XKB_STATE_NOTIFY: {
            xcb_xkb_state_notify_event_t *state_notify_event = (xcb_xkb_state_notify_event_t *)event;
            UpdateLockedModifiers(state_notify_event);
            return;
         }
      }
   }
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
         UpdateDepressedModifiers(keyEvent.code.value, true);
         keyEvent.code.modifiers = m_Mods;

         keyEvent.keyName = magic_enum::enum_name(keyEvent.code.value);
         keyEvent.sourceWindow = GetSubscribedWindows().at(pressEvent->event).lock()->GetGenericID();
         if (m_InternalKeyState[pressEvent->detail] == true) {
            keyEvent.pressType = KeyPressType::REPEAT;
         } else {
            keyEvent.pressType = KeyPressType::PRESSED;
            m_InternalKeyState[pressEvent->detail] = true;
         }
         // Handle CharacterEvents last.
         char character = (char)xkb_state_key_get_utf32(m_realState, pressEvent->detail);
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
         UpdateDepressedModifiers(keyEvent.code.value, false);
         keyEvent.code.modifiers = m_Mods;

         keyEvent.keyName = magic_enum::enum_name(keyEvent.code.value);
         keyEvent.pressType = KeyPressType::RELEASED;
         keyEvent.sourceWindow = GetSubscribedWindows().at(releaseEvent->event).lock()->GetGenericID();
         m_InternalKeyState[releaseEvent->detail] = false;
         break;
      }
   }
   return keyEvent;
}

void X11Keyboard::UpdateDepressedModifiers(NLSWIN::KeyValue val, bool pressed) {
   xkb_mod_mask_t depressedMods = xkb_state_serialize_mods(m_realState, XKB_STATE_MODS_DEPRESSED);
   xkb_mod_mask_t latchedMods = xkb_state_serialize_mods(m_realState, XKB_STATE_MODS_LATCHED);
   xkb_mod_mask_t lockedMods = xkb_state_serialize_mods(m_realState, XKB_STATE_MODS_LOCKED);
   if ((val == NLSWIN::KeyValue::KEY_LSHIFT) || (val == NLSWIN::KeyValue::KEY_RSHIFT)) {
      m_Mods.shift = pressed;
      if (pressed) {
         depressedMods |= XCB_MOD_MASK_SHIFT;
      } else {
         depressedMods &= ~(XCB_MOD_MASK_SHIFT);
      }
   } else if ((val == NLSWIN::KeyValue::KEY_LCTRL) || (val == NLSWIN::KeyValue::KEY_RCTRL)) {
      m_Mods.ctrl = pressed;
      if (pressed) {
         depressedMods |= XCB_MOD_MASK_CONTROL;
      } else {
         depressedMods &= ~(XCB_MOD_MASK_CONTROL);
      }
   } else if ((val == NLSWIN::KeyValue::KEY_LALT) || (val == NLSWIN::KeyValue::KEY_RALT)) {
      m_Mods.alt = pressed;
      if (pressed) {
         depressedMods |= XCB_MOD_MASK_1;
      } else {
         depressedMods &= ~(XCB_MOD_MASK_1);
      }
   } else if ((val == NLSWIN::KeyValue::KEY_LALT) || (val == NLSWIN::KeyValue::KEY_RALT)) {
      m_Mods.super = pressed;
      if (pressed) {
         depressedMods |= XCB_MOD_MASK_4;
      } else {
         depressedMods &= ~(XCB_MOD_MASK_4);
      }
   }

   xkb_state_update_mask(m_realState, depressedMods, latchedMods, lockedMods, 0, 0, 0);
}

void X11Keyboard::UpdateLockedModifiers(xcb_xkb_state_notify_event_t *stateNotify) {
   xkb_mod_mask_t depressedMods = xkb_state_serialize_mods(m_realState, XKB_STATE_MODS_DEPRESSED);
   xkb_mod_mask_t latchedMods = xkb_state_serialize_mods(m_realState, XKB_STATE_MODS_LATCHED);
   xkb_mod_mask_t lockedMods = xkb_state_serialize_mods(m_realState, XKB_STATE_MODS_LOCKED);

   m_Mods.capsLock = stateNotify->mods & XCB_MOD_MASK_LOCK;
   m_Mods.numLock = stateNotify->mods & XCB_MOD_MASK_2;
   m_Mods.scrollLock = stateNotify->mods & XCB_MOD_MASK_5;

   if (stateNotify->mods & XCB_MOD_MASK_LOCK) {
      lockedMods |= XCB_MOD_MASK_LOCK;
   } else {
      lockedMods &= ~(XCB_MOD_MASK_LOCK);
   }

   if (stateNotify->mods & XCB_MOD_MASK_2) {
      lockedMods |= XCB_MOD_MASK_2;
   } else {
      lockedMods &= ~(XCB_MOD_MASK_2);
   }

   if (stateNotify->mods & XCB_MOD_MASK_5) {
      lockedMods |= XCB_MOD_MASK_5;
   } else {
      lockedMods &= ~(XCB_MOD_MASK_5);
   }

   xkb_state_update_mask(m_realState, depressedMods, latchedMods, lockedMods, 0, 0, 0);
   if (m_Mods.numLock) {
      xkb_state_update_mask(m_dummyState, 0, 0, XCB_MOD_MASK_2, 0, 0, 0);
   } else {
      xkb_state_update_mask(m_dummyState, 0, 0, 0, 0, 0, 0);
   }
}

xkb_keysym_t X11Keyboard::GetSymFromKeyCode(unsigned int keycode) {
   // TODO: Consider somehow converting this to xcb.
   const xkb_keysym_t *array;
   return xkb_state_key_get_one_sym(m_dummyState, keycode);
}