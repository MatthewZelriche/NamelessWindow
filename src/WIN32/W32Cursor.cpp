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

W32Cursor::W32Cursor() {
}

void W32Cursor::BindToWindow(const Window* const window) noexcept {
}

void W32Cursor::UnbindFromWindows() noexcept {
}

void W32Cursor::ShowCursor() noexcept {
}

void W32Cursor::HideCursor() noexcept {
}

void W32Cursor::ProcessGenericEvent(MSG event) {
   // Always remember to convert the WParam to our special type
   // @see WParamWithWindowHandle
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