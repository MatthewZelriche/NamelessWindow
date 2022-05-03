#include <iostream>

#include "NamelessWindow/Events/EventBus.hpp"
#include "NamelessWindow/Window.hpp"
#include "NamelessWindow/Keyboard.hpp"

/** Test basic window events (WindowFocused, WindowResized, WindowRepositioned) and functions that can generate those events. */
using namespace NLSWIN;

int main() {
   WindowProperties props;
   props.xCoordinate = 1000;
   props.yCoordinate = 0;

   auto nonStandardWindow = Window::Create(props);
   auto standardWindow = Window::Create();
   nonStandardWindow->Show();
   standardWindow->Show();

   auto kb = Keyboard::Create();
   kb->SubscribeToWindow(standardWindow);
   kb->SubscribeToWindow(nonStandardWindow);

   while (nonStandardWindow || standardWindow) {
      NLSWIN::EventBus::PollEvents();
      if (nonStandardWindow && nonStandardWindow->RequestedClose()) {
         nonStandardWindow.reset();
      }
      if (standardWindow && standardWindow->RequestedClose()) {
         standardWindow.reset();
      }

      while (nonStandardWindow && nonStandardWindow->HasEvent()) { 
          auto event = nonStandardWindow->GetNextEvent();
          if (auto focusEvent = std::get_if<WindowFocusedEvent>(&event)) {
             std::cout << "Window " << nonStandardWindow->GetGenericID() << ": Received FocusEvent."
                       << std::endl;
          } else if (auto resizeEvent = std::get_if<WindowResizeEvent>(&event)) {
             std::cout << "Window " << nonStandardWindow->GetGenericID()
                       << ": resized to X = " << resizeEvent->newWidth << ", Y = " << resizeEvent->newHeight
                       << std::endl;
          } else if (auto rePosEvent = std::get_if<WindowRepositionEvent>(&event)) {
             std::cout << "Window " << nonStandardWindow->GetGenericID()
                       << ": moved to X = " << rePosEvent->newX << ", Y = " << rePosEvent->newY << std::endl;
          }
      }

      while (standardWindow && standardWindow->HasEvent()) {
         auto event = standardWindow->GetNextEvent();
         if (auto focusEvent = std::get_if<WindowFocusedEvent>(&event)) {
            std::cout << "Window " << standardWindow->GetGenericID() << ": Received FocusEvent."
                      << std::endl;
         } else if (auto resizeEvent = std::get_if<WindowResizeEvent>(&event)) {
            std::cout << "Window " << standardWindow->GetGenericID()
                      << ": resized to X = " << resizeEvent->newWidth << ", Y = " << resizeEvent->newHeight
                      << std::endl;
         }
      }

      while (kb->HasEvent()) { 
          auto event = kb->GetNextEvent();
          if (auto keyEvent = std::get_if<KeyEvent>(&event)) {
             switch (keyEvent->code.value) {
                case KEY_1: {
                   standardWindow->DisableUserResizing();
                   break;
                }
                case KEY_2: {
                   standardWindow->EnableUserResizing();
                   break;
                }
                case KEY_3: {
                   standardWindow->Reposition(500, 500);
                   break;
                }
                case KEY_4: {
                   standardWindow->Resize(1024, 768);
                   break;
                }
                case KEY_5: {
                   standardWindow->SetFullscreen(false);
                   break;
                }
                case KEY_6: {
                   standardWindow->SetFullscreen(true);
                   break;
                }
                case KEY_7: {
                   standardWindow->SetWindowed();
                   break;
                }
             }
         }
      }
   }
}