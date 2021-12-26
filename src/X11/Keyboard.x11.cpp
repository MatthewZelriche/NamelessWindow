#include "Keyboard.x11.hpp"

#include <cstring>

#include "EventQueue.x11.hpp"
#include "InputDevice.x11.hpp"
#include "InputMapper.x11.hpp"
#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/Window.hpp"
#include "Window.x11.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::vector<KeyboardDeviceInfo> Keyboard::EnumerateKeyboards() noexcept {
   std::vector<KeyboardDeviceInfo> keyboards;
   // Open a brief temporary connection to get the screens
   xcb_connection_t *connection = xcb_connect(nullptr, nullptr);
   int result = xcb_connection_has_error(connection);
   if (result != 0) {
      xcb_disconnect(connection);
      return {};
   }
   xcb_input_xi_query_device_cookie_t queryCookie =
      xcb_input_xi_query_device(connection, XCB_INPUT_DEVICE_ALL);
   xcb_input_xi_query_device_reply_t *reply =
      xcb_input_xi_query_device_reply(connection, queryCookie, nullptr);

   xcb_input_xi_device_info_iterator_t iter = xcb_input_xi_query_device_infos_iterator(reply);
   while (iter.rem > 0) {
      auto element = iter.data;
      if (element->enabled) {
         if (element->type == XCB_INPUT_DEVICE_TYPE_SLAVE_KEYBOARD) {
            // Ignore xtest devices
            const char *name = xcb_input_xi_device_info_name(element);
            if (!std::strstr(name, "XTEST")) {
               KeyboardDeviceInfo dev {name, element->deviceid};
               keyboards.push_back(dev);
            }
         }
      }
      xcb_input_xi_device_info_next(&iter);
   }
   free(reply);
   return keyboards;
}

void Keyboard::Impl::ProcessXInputEvent(xcb_ge_generic_event_t *event) {
   switch (event->event_type) {
      case XCB_INPUT_KEY_RELEASE:
      case XCB_INPUT_KEY_PRESS: {
         xcb_input_key_press_event_t *keyEvent = reinterpret_cast<xcb_input_key_press_event_t *>(event);
         if (m_SubscribedWindows.count(keyEvent->event)) {
            if (GetDeviceID() == keyEvent->deviceid || GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER) {
               Event processedEvent = ProcessKeyEvent(event);
               PushEvent(processedEvent);
            }
         }
      }
   }
}

Event Keyboard::Impl::ProcessKeyEvent(xcb_ge_generic_event_t *event) {
   KeyEvent keyEvent;
   switch (event->event_type) {
      case XCB_INPUT_KEY_PRESS: {
         xcb_input_key_press_event_t *pressEvent = reinterpret_cast<xcb_input_key_press_event_t *>(event);
         keyEvent.code.value = X11InputMapper::TranslateKey(GetSymFromKeyCode(pressEvent->detail));
         keyEvent.keyName = magic_enum::enum_name(keyEvent.code.value);
         keyEvent.code.modifiers = ParseModifierState(pressEvent->mods.base);
         keyEvent.sourceWindow = m_SubscribedWindows[pressEvent->event];
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
         keyEvent.sourceWindow = m_SubscribedWindows[releaseEvent->event];
         m_InternalKeyState[releaseEvent->detail] = false;
         break;
      }
   }
   return keyEvent;
}

KeyModifiers Keyboard::Impl::ParseModifierState(uint32_t mods) {
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

xkb_keysym_t Keyboard::Impl::GetSymFromKeyCode(unsigned int keycode) {
   // TODO: Consider somehow converting this to xcb even though xcb's lack of documentation makes me weep.
   return xkb_state_key_get_one_sym(m_KeyboardState, keycode);
}

Keyboard::Impl::Impl() {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();
   xkb_x11_setup_xkb_extension(m_connection, XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION,
                               XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, nullptr, nullptr);
   Init(xkb_x11_get_core_keyboard_device_id(m_connection));
}

Keyboard::Impl::Impl(KeyboardDeviceInfo device) {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();
   xkb_x11_setup_xkb_extension(m_connection, XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION,
                               XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, nullptr, nullptr);
   Init(device.platformSpecificIdentifier);
}
void Keyboard::Impl::Init(xcb_input_device_id_t deviceID) {
   m_subscribedMasks =
      (xcb_input_xi_event_mask_t)(XCB_INPUT_XI_EVENT_MASK_KEY_PRESS | XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE);
   m_keyboardContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
   m_deviceID = deviceID;
   auto deviceKeymap = xkb_x11_keymap_new_from_device(m_keyboardContext, m_connection, m_deviceID,
                                                      XKB_KEYMAP_COMPILE_NO_FLAGS);
   m_KeyboardState = xkb_x11_state_new_from_device(deviceKeymap, m_connection, m_deviceID);
}

Keyboard::Keyboard() : m_pImpl(std::make_shared<Keyboard::Impl>()) {
   EventQueueX11::RegisterListener(m_pImpl);
}

Keyboard::Keyboard(KeyboardDeviceInfo device) : m_pImpl(std::make_shared<Keyboard::Impl>(device)) {
   EventQueueX11::RegisterListener(m_pImpl);
}

Keyboard::~Keyboard() {
}

bool Keyboard::HasEvent() const noexcept {
   return m_pImpl->HasEvent();
}

Event Keyboard::GetNextEvent() {
   return m_pImpl->GetNextEvent();
}

void Keyboard::SubscribeToWindow(const Window &window) {
   m_pImpl->SubscribeToWindow(window.m_pImpl->GetX11WindowID(), window.GetWindowID());
}