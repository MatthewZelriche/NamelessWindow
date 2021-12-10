#include "EventQueue.x11.hpp"

#include <X11/extensions/XInput2.h>
#include <xcb/xinput.h>

#include <cstring>

#include "NamelessWindow/Events/Key.hpp"
#include "XConnection.h"

using namespace NLSWIN;

std::map<std::weak_ptr<EventListenerX11>, std::unordered_set<EventType>, std::owner_less<>>
   EventQueueX11::m_listeners;
xcb_connection_t *EventQueueX11::m_connection = nullptr;

void EventQueueX11::GetOSEvents() {
   XConnection::CreateConnection();
   m_connection = XConnection::GetConnection();

   xcb_generic_event_t *event = nullptr;
   while (event = xcb_poll_for_event(m_connection)) {
      // Handle Xinput2 events first. Xinput2 events are designated XCB_GE_GENERIC.
      if ((event->response_type & ~80) == XCB_GE_GENERIC) {
         switch (((xcb_ge_event_t *)event)->event_type) {
            // TODO: Handle repeats.
            case XCB_INPUT_KEY_PRESS: {
               xcb_input_key_press_event_t *keyPress = reinterpret_cast<xcb_input_key_press_event_t *>(event);
               KeyEvent keyEvent;
               keyEvent.pressType = KeyPressType::PRESSED;
               DispatchEventToListeners(keyEvent, keyPress->event, KeyEvent::type, keyPress->deviceid);
               break;
            }
            case XCB_INPUT_KEY_RELEASE: {
               xcb_input_key_release_event_t *keyRelease =
                  reinterpret_cast<xcb_input_key_release_event_t *>(event);
               KeyEvent keyEvent;
               keyEvent.pressType = KeyPressType::RELEASED;
               DispatchEventToListeners(keyEvent, keyRelease->event, KeyEvent::type, keyRelease->deviceid);
               break;
            }
         }
         break;  // Break from the entire while loop, no need to run the second switch statement.
      }

      // Handle all non-Xinput2 events.
      switch (event->response_type & ~0x80) {
         case XCB_FOCUS_IN: {
            xcb_focus_in_event_t *focusEvent = reinterpret_cast<xcb_focus_in_event_t *>(event);
            WindowFocusedEvent windowFocusEvent;
            DispatchEventToListeners(windowFocusEvent, focusEvent->event, WindowFocusedEvent::type);
            break;
         }
         case XCB_CLIENT_MESSAGE: {
            // XCB_CLIENT_MESSAGE is currently only used for overriding the X11 window manager and handling a
            // close event directly. The close event is not sent to the API user, it is only handled
            // internally.
            xcb_client_message_event_t *clientEvent = reinterpret_cast<xcb_client_message_event_t *>(event);
            xcb_intern_atom_cookie_t deleteWindowCookie =
               xcb_intern_atom(m_connection, false, std::strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
            xcb_intern_atom_reply_t *deleteWindowAtomReply =
               xcb_intern_atom_reply(m_connection, deleteWindowCookie, nullptr);

            // Test if this is actually a close event.
            if (clientEvent->data.data32[0] == deleteWindowAtomReply->atom) {
               WindowCloseEvent closeEvent;
               DispatchEventToListeners(closeEvent, clientEvent->window, WindowCloseEvent::type);
               break;
            }
         }
      }
   }
}

void EventQueueX11::DispatchEventToListeners(Event event, xcb_window_t windowID, EventType eventType,
                                             xcb_input_device_id_t deviceID) {
   for (auto &listener: m_listeners) {
      if (listener.second.find(eventType) != listener.second.end()) {
         if (!listener.first.expired()) {
            auto listenerSharedPtr = listener.first.lock();
            const auto &interestedWindowSources = listenerSharedPtr->GetWindows();
            // Is the listener interested in events from this specific windowID?
            if (interestedWindowSources.find(windowID) != interestedWindowSources.end()) {
               if (deviceID == 0) {
                  // Not related to a specific deviceID, send to everyone interested.
                  listenerSharedPtr->EventRecieved(event);
               } else if (listenerSharedPtr->GetDeviceID() == XCB_INPUT_DEVICE_ALL_MASTER ||
                          listenerSharedPtr->GetDeviceID() == deviceID) {
                  // Send XInput2 events only to listeners who are interested in all devices, or listeners
                  // associated with a specific device.
                  listenerSharedPtr->EventRecieved(event);
               }
            }
         }
      }
   }
}

void EventQueueX11::RegisterForEvent(std::weak_ptr<EventListenerX11> listener, EventType eventType) {
   // Attempt to add the listener to the map with a default-initialized vector as its key.
   // If it already exists, does nothing.
   auto &vectorRef = m_listeners[listener];
   vectorRef.insert(eventType);
}

bool EventQueueX11::UnregisterForEvent(std::weak_ptr<EventListenerX11> listener, EventType eventType) {
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