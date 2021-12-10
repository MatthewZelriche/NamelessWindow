#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <queue>
#include <unordered_set>

#include "NamelessWindow/Events/Event.hpp"
#include "NamelessWindow/NLSAPI.h"

namespace NLSWIN {

class NLSWIN_API_PRIVATE EventListenerX11 {
   private:
   std::queue<Event> m_Queue;

   protected:
   std::unordered_set<xcb_window_t> m_windows;
   xcb_input_device_id_t m_deviceID = XCB_INPUT_DEVICE_ALL_MASTER;

   public:
   [[nodiscard]] bool HasEvent() const noexcept;
   Event GetNextEvent();
   virtual void EventRecieved(Event event);

   [[nodiscard]] inline const std::unordered_set<xcb_window_t> &GetWindows() const { return m_windows; }
   [[nodiscard]] inline xcb_input_device_id_t GetDeviceID() const { return m_deviceID; }
};

}  // namespace NLSWIN