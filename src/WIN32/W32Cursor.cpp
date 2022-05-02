#include "W32Cursor.hpp"

#include <windowsx.h>

#include <memory>

#include "Events/W32EventBus.hpp"
#include "NamelessWindow/Cursor.hpp"

using namespace NLSWIN;

std::shared_ptr<Cursor> Cursor::Create() {
   std::shared_ptr<W32Cursor> impl = std::make_shared<W32Cursor>();
   W32EventBus::GetInstance().RegisterListener(impl);
   return std::move(impl);
}

void W32Cursor::BindToWindow(Window* window) noexcept {
   auto w32Window = reinterpret_cast<W32Window*>(window);

   // If we are attempting to rebind to the same window, no need to do anything.
   if (w32Window->GetGenericID() != m_boundWindow.first) {
      // Set the window we are meant to be bound to when the window is focused.
      m_boundWindow.first = w32Window->GetGenericID();
      m_boundWindow.second = w32Window->GetWin32Handle();
      RECT rect;
      GetClientRect(w32Window->GetWin32Handle(), &rect);
      ConfineCursorToRect(w32Window->GetWin32Handle(), rect);
      w32Window->Focus();
   }
}

void W32Cursor::ConfineCursorToRect(HWND handle, RECT rect) {
   // If the bound window isn't focused, disregard this attempt to clip the cursor.
   if ((W32Window::IDFromHWND(handle) != m_focusedWindow)) {
      return;
   }
   POINT topLeft {rect.left, rect.top};
   ClientToScreen(handle, &topLeft);

   POINT bottomRight {rect.right, rect.bottom};
   ClientToScreen(handle, &bottomRight);

   RECT screenRect {topLeft.x, topLeft.y, bottomRight.x, bottomRight.y};
   ClipCursor(&screenRect);
}

void W32Cursor::UnbindFromWindows() noexcept {
   if (m_boundWindow.first) {
      ClipCursor(nullptr);
      m_boundWindow = {0, 0};
   }
}

void W32Cursor::Show() noexcept {
   m_hideCursor = false;
   SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), CURSOR_VISIBILITY, (WPARAM)true, 0);
}

void W32Cursor::Hide() noexcept {
   m_hideCursor = true;
   if (m_focusedWindow) {
      if (m_hideCursor) {
         SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), CURSOR_VISIBILITY, (WPARAM)false, 0);
      }
   }
}

void W32Cursor::ProcessGenericEvent(MSG event) {
   //  Always remember to convert the WParam to our special type
   //  @see WParamWithWindowHandle
   WParamWithWindowHandle* wParam = reinterpret_cast<WParamWithWindowHandle*>(event.wParam);
   switch (event.message) {
      case WM_INPUT: {
         // WM_INPUT is a special case, we we transform the raw input on the event thread.
         // This requires a heap allocation and we must be careful to delete it at the end of this method.
         // See W32EventThreadDispatcher for details.
         RAWINPUT* inputStruct = reinterpret_cast<RAWINPUT*>(event.lParam);
         if (inputStruct->header.dwType == RIM_TYPEMOUSE) {
            if (inputStruct->data.mouse.lLastX || inputStruct->data.mouse.lLastY) {
               PushEvent(PackageRawDeltaEvent(inputStruct->data.mouse));
            }
         }
      } break;
      case WM_XBUTTONDOWN: {
         if (GET_XBUTTON_WPARAM(wParam->wParam) == XBUTTON1) {
            PushEvent(
               PackageButtonEvent(event, ButtonValue::MB_4, ButtonPressType::PRESSED, wParam->sourceWindow));
            break;
         } else {
            PushEvent(
               PackageButtonEvent(event, ButtonValue::MB_5, ButtonPressType::PRESSED, wParam->sourceWindow));
            break;
         }
      }
      case WM_XBUTTONUP: {
         if (GET_XBUTTON_WPARAM(wParam->wParam) == XBUTTON1) {
            PushEvent(
               PackageButtonEvent(event, ButtonValue::MB_4, ButtonPressType::RELEASED, wParam->sourceWindow));
            break;
         } else {
            PushEvent(
               PackageButtonEvent(event, ButtonValue::MB_5, ButtonPressType::RELEASED, wParam->sourceWindow));
            break;
         }
      }
      case WM_LBUTTONDOWN: {
         PushEvent(PackageButtonEvent(event, ButtonValue::LEFTCLICK, ButtonPressType::PRESSED,
                                      wParam->sourceWindow));
         break;
      }
      case WM_LBUTTONUP: {
         PushEvent(PackageButtonEvent(event, ButtonValue::LEFTCLICK, ButtonPressType::RELEASED,
                                      wParam->sourceWindow));
         break;
      }
      case WM_MBUTTONDOWN: {
         PushEvent(PackageButtonEvent(event, ButtonValue::MIDDLECLICK, ButtonPressType::PRESSED,
                                      wParam->sourceWindow));
         break;
      }
      case WM_MBUTTONUP: {
         PushEvent(PackageButtonEvent(event, ButtonValue::MIDDLECLICK, ButtonPressType::RELEASED,
                                      wParam->sourceWindow));
         break;
      }
      case WM_RBUTTONDOWN: {
         PushEvent(PackageButtonEvent(event, ButtonValue::RIGHTCLICK, ButtonPressType::PRESSED,
                                      wParam->sourceWindow));
         break;
      }
      case WM_RBUTTONUP: {
         PushEvent(PackageButtonEvent(event, ButtonValue::RIGHTCLICK, ButtonPressType::RELEASED,
                                      wParam->sourceWindow));
         break;
      }
      case WM_MOUSEWHEEL: {
         MouseScrollEvent scrollEvent;
         if (GET_WHEEL_DELTA_WPARAM(wParam->wParam) > 0) {
            scrollEvent.scrollType = ScrollType::UP;
         } else {
            scrollEvent.scrollType = ScrollType::DOWN;
         }
         scrollEvent.sourceWindow = W32Window::IDFromHWND(wParam->sourceWindow);

         // Need to convert to client coordinates.
         POINT coords {GET_X_LPARAM(event.lParam), GET_Y_LPARAM(event.lParam)};
         ScreenToClient(wParam->sourceWindow, &coords);
         scrollEvent.xPos = coords.x;
         scrollEvent.yPos = coords.y;

         // Scroll wheel events normally trigger on the ENTIRE window, including non-client area.
         // We need to avoid this to conform to the NLSWIN API.
         RECT clientRect;
         GetClientRect(wParam->sourceWindow, &clientRect);
         if (scrollEvent.xPos < 0 || scrollEvent.xPos > clientRect.right || scrollEvent.yPos < 0 ||
             scrollEvent.yPos > clientRect.bottom) {
            break;
         }
         PushEvent(scrollEvent);
         break;
      }
      case WM_MOUSEMOVE: {
         MouseMovementEvent moveEvent;
         moveEvent.sourceWindow = W32Window::IDFromHWND(wParam->sourceWindow);
         moveEvent.newXPos = GET_X_LPARAM(event.lParam);
         moveEvent.newYPos = GET_Y_LPARAM(event.lParam);
         PushEvent(moveEvent);

         if (m_inhabitedWindow != moveEvent.sourceWindow) {
            // Set up a tracking event for our next leave event.
            TRACKMOUSEEVENT trackInfo {0};
            trackInfo.cbSize = sizeof(TRACKMOUSEEVENT);
            trackInfo.dwFlags = TME_LEAVE;
            trackInfo.hwndTrack = wParam->sourceWindow;
            TrackMouseEvent(&trackInfo);

            // Set the new inhabited window.
            m_inhabitedWindow = moveEvent.sourceWindow;

            // Send a MouseEnter event.
            MouseEnterEvent enterEvent;
            enterEvent.sourceWindow = m_inhabitedWindow;
            enterEvent.xPos = moveEvent.newXPos;
            enterEvent.yPos = moveEvent.newYPos;
            PushEvent(enterEvent);

            // Handle cursor hiding.
            if (m_hideCursor) {
               SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), CURSOR_VISIBILITY, (WPARAM)false,
                            0);
            }
         }
         break;
      }
      case WM_MOUSELEAVE: {
         MouseLeaveEvent leaveEvent;
         leaveEvent.sourceWindow = W32Window::IDFromHWND(wParam->sourceWindow);
         m_inhabitedWindow = 0;
         PushEvent(leaveEvent);

         // Handle cursor hiding
         if (m_hideCursor) {
            SendMessageW(W32EventThreadDispatcher::GetDispatcherHandle(), CURSOR_VISIBILITY, (WPARAM)true,
                         0);
         }
         break;
      }
      case WM_EXITSIZEMOVE: {
         // This message is received when the user has stopped click-and-drag operation on the nonclient area.
         // We block attempts to confine the cursor until the user is done this operation.
         m_blockCursorClip = false;
         m_beginClickOnNCArea = false;
         if (wParam->sourceWindow == m_boundWindow.second) {
            // Rebind the window now that the user is done their move or size operation.
            RECT rect;
            GetClientRect(m_boundWindow.second, &rect);
            ConfineCursorToRect(m_boundWindow.second, rect);
         }
         break;
      }
      case WM_NCMOUSEMOVE: {
         // This mess is because no matter what I do, I cannot get WM_NCLBUTTONUP events to occur
         // during a quick click-and-release of the title bar. This registers as a move, but never actually
         // calls WM_EXITSIZEMOVE, so we can't test for it there.
         // This event doesn't fire while the mouse button is held down, so we can exploit this to determine
         // when the user actually releases the mouse in the nc area. This doesn't work for testing for mouse
         // release after pressing the maximize button; see WM_NCLBUTTONDOWN.
         if (m_beginClickOnNCArea) {
            m_blockCursorClip = false;
            m_beginClickOnNCArea = false;
            if (wParam->sourceWindow == m_boundWindow.second) {
               // Rebind the window now that the user has completed a quick click-and-release, that
               // results in the bound window receiving focus.
               RECT rect;
               GetClientRect(m_boundWindow.second, &rect);
               ConfineCursorToRect(m_boundWindow.second, rect);
            }
         }
         break;
      }
      case WM_NCLBUTTONDOWN: {
         // This event is received when the user clicks down on the nc area. Tracking this is necessary for
         // the maximize button and certain quick clicks; see WM_NCMOUSEMOVE.
         m_beginClickOnNCArea = true;
         if (wParam->wParam == HTMAXBUTTON) {
            // Max button is a special case to ensure mouse is clipped after performing a "restore" op.
            m_beginClickOnNCArea = false;
            m_blockCursorClip = false;
         }
         break;
      }
      case WM_MOUSEACTIVATE: {
         // When looking for an activate, we do some additional processing here.
         // We prevent clipping of the cursor while the user is actively doing something with the nc area.
         if (LOWORD(event.lParam) != HTCLIENT) {
            m_blockCursorClip = true;
         }
         break;
      }
      case WM_SETFOCUS: {
         // Set the window with focus.
         m_focusedWindow = W32Window::IDFromHWND(wParam->sourceWindow);
         [[fallthrough]];
      }
      case WM_MOVE:
      case WM_SIZE: {
         if ((wParam->sourceWindow == m_boundWindow.second) && !m_blockCursorClip) {
            RECT rect;
            GetClientRect(m_boundWindow.second, &rect);
            ConfineCursorToRect(m_boundWindow.second, rect);
         }
         break;
      }
      case WM_KILLFOCUS: {
         // Release the clip while the bound window isn't focused.
         ClipCursor(nullptr);
         break;
      }
      case WM_DESTROY: {
         // Windows unlocks the cursor for us when the window is destroyed, but we need
         // to reset our internal tracking as well.
         if (wParam->sourceWindow == m_boundWindow.second) {
            UnbindFromWindows();
         }
         break;
      }
   }
}

Event W32Cursor::PackageButtonEvent(MSG event, ButtonValue value, ButtonPressType type, HWND window) {
   MouseButtonEvent buttonEvent;
   buttonEvent.button = value;
   buttonEvent.type = type;
   buttonEvent.sourceWindow = W32Window::IDFromHWND(window);
   buttonEvent.xPos = GET_X_LPARAM(event.lParam);
   buttonEvent.yPos = GET_Y_LPARAM(event.lParam);
   return buttonEvent;
}