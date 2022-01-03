#include "Keyboard.x11.hpp"

#include <cstring>

#include "EventDispatcher.x11.hpp"
#include "InputDevice.x11.hpp"
#include "InputMapper.x11.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Exceptions.hpp"
#include "NamelessWindow/Window.hpp"
#include "Window.x11.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::vector<KeyboardDeviceInfo> Keyboard::EnumerateKeyboards() noexcept {
   return EnumerateDevicesX11<KeyboardDeviceInfo>(XCB_INPUT_DEVICE_TYPE_SLAVE_KEYBOARD);
}

void KeyboardX11::ProcessXInputEvent(xcb_ge_generic_event_t *event) {
   switch (event->event_type) {
      case XCB_INPUT_KEY_RELEASE:
      case XCB_INPUT_KEY_PRESS: {
         xcb_input_key_press_event_t *keyEvent = reinterpret_cast<xcb_input_key_press_event_t *>(event);
         if (m_subscribedWindows.count(keyEvent->event)) {
            if (GetDeviceID() == keyEvent->deviceid || GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
               Event processedEvent = ProcessKeyEvent(event);
               PushEvent(processedEvent);
            }
         }
      }
   }
}

Event KeyboardX11::ProcessKeyEvent(xcb_ge_generic_event_t *event) {
   KeyEvent keyEvent;
   switch (event->event_type) {
      case XCB_INPUT_KEY_PRESS: {
         xcb_input_key_press_event_t *pressEvent = reinterpret_cast<xcb_input_key_press_event_t *>(event);
         keyEvent.code.value = X11InputMapper::TranslateKey(GetSymFromKeyCode(pressEvent->detail));
         keyEvent.keyName = magic_enum::enum_name(keyEvent.code.value);
         keyEvent.code.modifiers = ParseModifierState(pressEvent->mods.base);
         keyEvent.sourceWindow = m_subscribedWindows[pressEvent->event];
         if (m_InternalKeyState[pressEvent->detail] == true) {
            keyEvent.pressType = KeyPressType::REPEAT;
         } else {
            keyEvent.pressType = KeyPressType::PRESSED;
            m_InternalKeyState[pressEvent->detail] = true;
         }
         break;
      }
      case XCB_INPUT_KEY_RELEASE: {
         xcb_input_key_release_event_t *releaseEvent =
            reinterpret_cast<xcb_input_key_release_event_t *>(event);
         keyEvent.code.value = X11InputMapper::TranslateKey(GetSymFromKeyCode(releaseEvent->detail));
         keyEvent.keyName = magic_enum::enum_name(keyEvent.code.value);
         keyEvent.pressType = KeyPressType::RELEASED;
         keyEvent.sourceWindow = m_subscribedWindows[releaseEvent->event];
         m_InternalKeyState[releaseEvent->detail] = false;
         break;
      }
   }
   return keyEvent;
}

KeyModifiers KeyboardX11::ParseModifierState(uint32_t mods) {
   KeyModifiers modifiers {false};
   if ((mods & XCB_MOD_MASK_CONTROL)) {
      modifiers.ctrl = true;
   }
   if (mods & XCB_MOD_MASK_4) {
      modifiers.super = true;
   }
   if (mods & XCB_MOD_MASK_1) {
      modifiers.alt = true;
   }
   if (mods & XCB_MOD_MASK_SHIFT) {
      modifiers.shift = true;
   }
   if (mods & XCB_MOD_MASK_LOCK) {
      modifiers.capslock = true;
   }
   return modifiers;
}

xkb_keysym_t KeyboardX11::GetSymFromKeyCode(unsigned int keycode) {
   // TODO: Consider somehow converting this to xcb even though xcb's lack of documentation makes me weep.
   return xkb_state_key_get_one_sym(m_KeyboardState, keycode);
}

KeyboardX11::KeyboardX11() {
   m_connection = XConnection::GetConnection();
   xkb_x11_setup_xkb_extension(m_connection, XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION,
                               XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, nullptr, nullptr);
   Init(xkb_x11_get_core_keyboard_device_id(m_connection));
}

KeyboardX11::KeyboardX11(KeyboardDeviceInfo device) {
   m_connection = XConnection::GetConnection();
   bool result =
      xkb_x11_setup_xkb_extension(m_connection, XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION,
                                  XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, nullptr, nullptr);
   if (!result) {
      throw PlatformInitializationException();
   }
   Init(device.platformSpecificIdentifier);
}
void KeyboardX11::Init(xcb_input_device_id_t deviceID) {
   m_keyboardContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
   m_deviceID = deviceID;
   auto deviceKeymap = xkb_x11_keymap_new_from_device(m_keyboardContext, m_connection, m_deviceID,
                                                      XKB_KEYMAP_COMPILE_NO_FLAGS);
   m_KeyboardState = xkb_x11_state_new_from_device(deviceKeymap, m_connection, m_deviceID);
   xkb_keymap_unref(deviceKeymap);
}
std::shared_ptr<Keyboard> Keyboard::Create() {
   std::shared_ptr<KeyboardX11> impl = std::make_shared<KeyboardX11>();
   EventDispatcherX11::RegisterListener(impl);
   return std::move(impl);
}

std::shared_ptr<Keyboard> Keyboard::Create(KeyboardDeviceInfo device) {
   std::shared_ptr<KeyboardX11> impl = std::make_shared<KeyboardX11>(device);
   EventDispatcherX11::RegisterListener(impl);
   return std::move(impl);
}

void KeyboardX11::SubscribeToWindow(const Window *const window) noexcept {
   auto windowImpl = static_cast<const WindowX11 *const>(window);
   InputDeviceX11::SubscribeToWindow(
      windowImpl->GetX11WindowID(), windowImpl->GetWindowID(),
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_KEY_PRESS | XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE));
}

KeyboardX11::~KeyboardX11() {
   xkb_context_unref(m_keyboardContext);
   xkb_state_unref(m_KeyboardState);
}