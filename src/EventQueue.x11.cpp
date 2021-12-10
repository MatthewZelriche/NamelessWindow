#include "EventQueue.x11.hpp"

#include <X11/extensions/XInput2.h>
#include <xcb/xinput.h>

#include <cstring>

#include "NamelessWindow/Key.hpp"
#include "NamelessWindow/exceptions.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::map<std::weak_ptr<EventListenerX11>, std::unordered_set<std::type_index>, std::owner_less<>>
   EventQueueX11::m_listeners;
xcb_connection_t *EventQueueX11::m_connection = nullptr;

#include <iostream>
void EventQueueX11::GetOSEvents() {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();

   xcb_generic_event_t *event = nullptr;
   while (event = xcb_poll_for_event(m_connection)) {
      if ((event->response_type & ~80) == XCB_GE_GENERIC) {
         switch (((xcb_ge_event_t *)event)->event_type) {
         // TODO: Handle repeats.
         case XCB_INPUT_KEY_PRESS: {
            xcb_input_key_press_event_t *keyPress = (xcb_input_key_press_event_t *)event;
            for (auto listener: m_listeners) {
               auto listenerSharedPtr = listener.first.lock();
               auto &windows          = listenerSharedPtr->GetWindows();
               if (windows.find(keyPress->event) != windows.end()) {
                  KeyEvent keyEvent;
                  keyEvent.pressType = KeyPressType::PRESSED;
                  listenerSharedPtr->EventRecieved(keyEvent);
               }
            }
            break;
         }
         case XCB_INPUT_KEY_RELEASE: {
            xcb_input_key_release_event_t *keyRelease = (xcb_input_key_release_event_t *)event;
            for (auto listener: m_listeners) {
               auto listenerSharedPtr = listener.first.lock();
               auto &windows          = listenerSharedPtr->GetWindows();
               if (windows.find(keyRelease->event) != windows.end()) {
                  KeyEvent keyEvent;
                  keyEvent.pressType = KeyPressType::RELEASED;
                  listenerSharedPtr->EventRecieved(keyEvent);
               }
            }
            break;
         }
         }
      }

      switch (event->response_type & ~0x80) {
      case XCB_FOCUS_IN: {
         xcb_focus_in_event_t *focusEvent = reinterpret_cast<xcb_focus_in_event_t *>(event);
         for (auto listener: m_listeners) {
            auto listenerSharedPtr = listener.first.lock();
            auto &windows          = listenerSharedPtr->GetWindows();
            if (windows.find(focusEvent->event) != windows.end()) {
               listenerSharedPtr->EventRecieved(WindowFocusedEvent());
            }
         }
         break;
      }
      case XCB_CLIENT_MESSAGE: {
         xcb_client_message_event_t *clientEvent = reinterpret_cast<xcb_client_message_event_t *>(event);
         xcb_intern_atom_cookie_t deleteWindowCookie =
            xcb_intern_atom(m_connection, false, std::strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
         xcb_intern_atom_reply_t *deleteWindowAtomReply =
            xcb_intern_atom_reply(m_connection, deleteWindowCookie, nullptr);
         if (clientEvent->data.data32[0] == deleteWindowAtomReply->atom) {
            for (auto listener: m_listeners) {
               if (!listener.first.expired()) {
                  auto listenerSharedPtr = listener.first.lock();
                  auto &windows          = listenerSharedPtr->GetWindows();
                  if (windows.find(clientEvent->window) != windows.end()) {
                     listenerSharedPtr->EventRecieved(WindowCloseEvent());
                  }
               }
            }
         }
      }
      }
   }
}

void EventQueueX11::RegisterXConnection(xcb_connection_t *connection) {
   if (m_connection) {
      return;
   }
   m_connection = connection;
}

void EventQueueX11::RegisterForEvent(std::weak_ptr<EventListenerX11> listener, std::type_index eventType) {
   // Attempt to add the listener to the map with a default-initialized vector as its key.
   // If it already exists, does nothing.
   auto &vectorRef = m_listeners[listener];
   vectorRef.insert(eventType);
}

bool EventQueueX11::UnregisterForEvent(std::weak_ptr<EventListenerX11> listener, std::type_index eventType) {
   if (m_listeners.find(listener) != m_listeners.end()) {
      auto &vectorRef = m_listeners[listener];
      if (vectorRef.erase(eventType) == 1) {
         return true;
      } else {
         return false;
      }
   } else {
      return false;
   }
}

void EventQueue::GetOSEvents() {
   EventQueueX11::GetOSEvents();
}