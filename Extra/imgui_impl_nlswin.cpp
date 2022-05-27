#include "imgui_impl_nlswin.hpp"

#include <NamelessWindow/Events/Event.hpp>

static ImGuiKey TranslateKey(NLSWIN::KeyValue code);
static int TranslateMouseButton(NLSWIN::ButtonValue val);

bool ImGui_ImplNLSWin_Init(std::weak_ptr<NLSWIN::Window> window) {
   ImGuiIO &io = ImGui::GetIO();
   IM_ASSERT(io.BackendPlatformUserData == nullptr);

   ImGui_ImplNLSWin_Data *data = IM_NEW(ImGui_ImplNLSWin_Data)();
   data->window = window;

   io.BackendPlatformUserData = (void *)data;
   io.BackendPlatformName = "IMGUI_NLSWIN";
   return true;
}

void ImGui_ImplNLSWin_NewFrame() {
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplNLSWin_Data *data = (ImGui_ImplNLSWin_Data *)(io.BackendPlatformUserData);
   IM_ASSERT(data != nullptr && !data->window.expired());
   auto ptr = data->window.lock();
   io.DisplaySize = {(float)ptr->GetWindowWidth(), (float)ptr->GetWindowHeight()};

   if (data->lastFrameTime != nullLastFrame) {
      auto currentFrameTime = std::chrono::steady_clock::now();
      double deltaTimeUs =
         std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - data->lastFrameTime)
            .count();
      io.DeltaTime = deltaTimeUs * 1000000;
   } else {
      io.DeltaTime = 1.0f / 60.0f;
   }
}

void ImGui_ImplNLSWin_HandleEvent(NLSWIN::Event ev) {
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplNLSWin_Data *data = (ImGui_ImplNLSWin_Data *)(io.BackendPlatformUserData);
   IM_ASSERT(data != nullptr);

   if (auto event = std::get_if<NLSWIN::MouseMovementEvent>(&ev)) {
      io.AddMousePosEvent(event->newXPos, event->newYPos);
   } else if (auto event = std::get_if<NLSWIN::MouseButtonEvent>(&ev)) {
      io.AddMouseButtonEvent(TranslateMouseButton(event->button),
                             event->type == NLSWIN::ButtonPressType::PRESSED);
   } else if (auto event = std::get_if<NLSWIN::MouseScrollEvent>(&ev)) {
      float dir = event->scrollType == NLSWIN::ScrollType::UP ? 1.0f : -1.0f;
      io.AddMouseWheelEvent(0.0f, dir);
   } else if (auto event = std::get_if<NLSWIN::KeyEvent>(&ev)) {
      if (event->pressType == NLSWIN::KeyPressType::PRESSED) {
         io.AddKeyEvent(TranslateKey(event->code.value), true);
      } else if (event->pressType == NLSWIN::KeyPressType::RELEASED) {
         io.AddKeyEvent(TranslateKey(event->code.value), false);
      }
      io.AddKeyEvent(ImGuiKey_ModShift, event->code.modifiers.shift);
      io.AddKeyEvent(ImGuiKey_ModAlt, event->code.modifiers.alt);
      io.AddKeyEvent(ImGuiKey_ModCtrl, event->code.modifiers.ctrl);
      io.AddKeyEvent(ImGuiKey_ModSuper, event->code.modifiers.super);
   } else if (auto event = std::get_if<NLSWIN::CharacterEvent>(&ev)) {
      io.AddInputCharacter(event->character);
   }
}

void ImGui_ImplNLSWin_Shutdown() {
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplNLSWin_Data *data = (ImGui_ImplNLSWin_Data *)(io.BackendPlatformUserData);
   IM_ASSERT(data != nullptr);

   io.BackendPlatformUserData = nullptr;
   io.BackendPlatformName = nullptr;
}

static int TranslateMouseButton(NLSWIN::ButtonValue val) {
   switch (val) {
      case NLSWIN::ButtonValue::LEFTCLICK:
         return 0;
      case NLSWIN::ButtonValue::RIGHTCLICK:
         return 1;
      case NLSWIN::ButtonValue::MIDDLECLICK:
         return 2;
      case NLSWIN::ButtonValue::MB_4:
         return 3;
      case NLSWIN::ButtonValue::MB_5:
         return 4;
   }
}

static ImGuiKey TranslateKey(NLSWIN::KeyValue code) {
   switch (code) {
      case NLSWIN::KeyValue::KEY_NULL:
         return ImGuiKey_None;
      case NLSWIN::KeyValue::KEY_0:
         return ImGuiKey_0;
      case NLSWIN::KeyValue::KEY_1:
         return ImGuiKey_1;
      case NLSWIN::KeyValue::KEY_2:
         return ImGuiKey_2;
      case NLSWIN::KeyValue::KEY_3:
         return ImGuiKey_3;
      case NLSWIN::KeyValue::KEY_4:
         return ImGuiKey_4;
      case NLSWIN::KeyValue::KEY_5:
         return ImGuiKey_5;
      case NLSWIN::KeyValue::KEY_6:
         return ImGuiKey_6;
      case NLSWIN::KeyValue::KEY_7:
         return ImGuiKey_7;
      case NLSWIN::KeyValue::KEY_8:
         return ImGuiKey_8;
      case NLSWIN::KeyValue::KEY_9:
         return ImGuiKey_9;
      case NLSWIN::KeyValue::KEY_A:
         return ImGuiKey_A;
      case NLSWIN::KeyValue::KEY_B:
         return ImGuiKey_A;
      case NLSWIN::KeyValue::KEY_C:
         return ImGuiKey_C;
      case NLSWIN::KeyValue::KEY_D:
         return ImGuiKey_D;
      case NLSWIN::KeyValue::KEY_E:
         return ImGuiKey_E;
      case NLSWIN::KeyValue::KEY_F:
         return ImGuiKey_F;
      case NLSWIN::KeyValue::KEY_G:
         return ImGuiKey_G;
      case NLSWIN::KeyValue::KEY_H:
         return ImGuiKey_H;
      case NLSWIN::KeyValue::KEY_I:
         return ImGuiKey_I;
      case NLSWIN::KeyValue::KEY_J:
         return ImGuiKey_J;
      case NLSWIN::KeyValue::KEY_K:
         return ImGuiKey_K;
      case NLSWIN::KeyValue::KEY_L:
         return ImGuiKey_L;
      case NLSWIN::KeyValue::KEY_M:
         return ImGuiKey_M;
      case NLSWIN::KeyValue::KEY_N:
         return ImGuiKey_N;
      case NLSWIN::KeyValue::KEY_O:
         return ImGuiKey_O;
      case NLSWIN::KeyValue::KEY_P:
         return ImGuiKey_P;
      case NLSWIN::KeyValue::KEY_Q:
         return ImGuiKey_Q;
      case NLSWIN::KeyValue::KEY_R:
         return ImGuiKey_R;
      case NLSWIN::KeyValue::KEY_S:
         return ImGuiKey_S;
      case NLSWIN::KeyValue::KEY_T:
         return ImGuiKey_T;
      case NLSWIN::KeyValue::KEY_U:
         return ImGuiKey_U;
      case NLSWIN::KeyValue::KEY_V:
         return ImGuiKey_V;
      case NLSWIN::KeyValue::KEY_W:
         return ImGuiKey_W;
      case NLSWIN::KeyValue::KEY_X:
         return ImGuiKey_X;
      case NLSWIN::KeyValue::KEY_Y:
         return ImGuiKey_Y;
      case NLSWIN::KeyValue::KEY_Z:
         return ImGuiKey_Z;
      case NLSWIN::KeyValue::KEY_ESC:
         return ImGuiKey_Escape;
      case NLSWIN::KeyValue::KEY_LSHIFT:
         return ImGuiKey_LeftShift;
      case NLSWIN::KeyValue::KEY_RSHIFT:
         return ImGuiKey_RightShift;
      case NLSWIN::KeyValue::KEY_LCTRL:
         return ImGuiKey_LeftCtrl;
      case NLSWIN::KeyValue::KEY_RCTRL:
         return ImGuiKey_RightCtrl;
      case NLSWIN::KeyValue::KEY_LSUPER:
         return ImGuiKey_LeftSuper;
      case NLSWIN::KeyValue::KEY_RSUPER:
         return ImGuiKey_RightSuper;
      case NLSWIN::KeyValue::KEY_LALT:
         return ImGuiKey_LeftAlt;
      case NLSWIN::KeyValue::KEY_RALT:
         return ImGuiKey_RightAlt;
      case NLSWIN::KeyValue::KEY_SPACE:
         return ImGuiKey_Space;
      case NLSWIN::KeyValue::KEY_TAB:
         return ImGuiKey_Tab;
      case NLSWIN::KeyValue::KEY_COMMA:
         return ImGuiKey_Comma;
      case NLSWIN::KeyValue::KEY_PERIOD:
         return ImGuiKey_Period;
      case NLSWIN::KeyValue::KEY_FORWARDSLASH:
         return ImGuiKey_Slash;
      case NLSWIN::KeyValue::KEY_SEMICOLON:
         return ImGuiKey_Semicolon;
      case NLSWIN::KeyValue::KEY_APOSTROPHE:
         return ImGuiKey_Apostrophe;
      case NLSWIN::KeyValue::KEY_BACKSLASH:
         return ImGuiKey_Backslash;
      case NLSWIN::KeyValue::KEY_ENTER:
         return ImGuiKey_Enter;
      case NLSWIN::KeyValue::KEY_LBRACKET:
         return ImGuiKey_LeftBracket;
      case NLSWIN::KeyValue::KEY_RBRACKET:
         return ImGuiKey_RightBracket;
      case NLSWIN::KeyValue::KEY_DASH:
         return ImGuiKey_Minus;
      case NLSWIN::KeyValue::KEY_EQUALS:
         return ImGuiKey_Equal;
      case NLSWIN::KeyValue::KEY_BACKSPACE:
         return ImGuiKey_Backspace;
      case NLSWIN::KeyValue::KEY_TILDE:
         return ImGuiKey_GraveAccent;
      case NLSWIN::KeyValue::KEY_F1:
         return ImGuiKey_F1;
      case NLSWIN::KeyValue::KEY_F2:
         return ImGuiKey_F2;
      case NLSWIN::KeyValue::KEY_F3:
         return ImGuiKey_F3;
      case NLSWIN::KeyValue::KEY_F4:
         return ImGuiKey_F4;
      case NLSWIN::KeyValue::KEY_F5:
         return ImGuiKey_F5;
      case NLSWIN::KeyValue::KEY_F6:
         return ImGuiKey_F6;
      case NLSWIN::KeyValue::KEY_F7:
         return ImGuiKey_F7;
      case NLSWIN::KeyValue::KEY_F8:
         return ImGuiKey_F8;
      case NLSWIN::KeyValue::KEY_F9:
         return ImGuiKey_F9;
      case NLSWIN::KeyValue::KEY_F10:
         return ImGuiKey_F10;
      case NLSWIN::KeyValue::KEY_F11:
         ImGuiKey_F11;
      case NLSWIN::KeyValue::KEY_F12:
         ImGuiKey_F12;
      case NLSWIN::KeyValue::KEY_LEFT:
         return ImGuiKey_LeftArrow;
      case NLSWIN::KeyValue::KEY_RIGHT:
         return ImGuiKey_RightArrow;
      case NLSWIN::KeyValue::KEY_UP:
         return ImGuiKey_UpArrow;
      case NLSWIN::KeyValue::KEY_DOWN:
         return ImGuiKey_DownArrow;
      case NLSWIN::KeyValue::KEY_CAPSLOCK:
         return ImGuiKey_CapsLock;
      case NLSWIN::KeyValue::KEY_SCROLL_LOCK:
         return ImGuiKey_ScrollLock;
      case NLSWIN::KeyValue::KEY_NUMLOCK:
         return ImGuiKey_NumLock;
      case NLSWIN::KeyValue::KEY_PRINTSCREEN:
         return ImGuiKey_PrintScreen;
      case NLSWIN::KeyValue::KEY_PAUSE:
         return ImGuiKey_Pause;
      case NLSWIN::KeyValue::KEY_INSERT:
         return ImGuiKey_Insert;
      case NLSWIN::KeyValue::KEY_HOME:
         return ImGuiKey_Home;
      case NLSWIN::KeyValue::KEY_PAGEUP:
         return ImGuiKey_PageUp;
      case NLSWIN::KeyValue::KEY_PAGEDOWN:
         return ImGuiKey_PageDown;
      case NLSWIN::KeyValue::KEY_DELETE:
         return ImGuiKey_Delete;
      case NLSWIN::KeyValue::KEY_END:
         return ImGuiKey_End;
      case NLSWIN::KeyValue::KEY_CLEAR:
         return ImGuiKey_None;  // ????
      case NLSWIN::KeyValue::KEY_NUMPAD_0:
         return ImGuiKey_Keypad0;
      case NLSWIN::KeyValue::KEY_NUMPAD_1:
         return ImGuiKey_Keypad1;
      case NLSWIN::KeyValue::KEY_NUMPAD_2:
         return ImGuiKey_Keypad2;
      case NLSWIN::KeyValue::KEY_NUMPAD_3:
         return ImGuiKey_Keypad3;
      case NLSWIN::KeyValue::KEY_NUMPAD_4:
         return ImGuiKey_Keypad4;
      case NLSWIN::KeyValue::KEY_NUMPAD_5:
         return ImGuiKey_Keypad5;
      case NLSWIN::KeyValue::KEY_NUMPAD_6:
         return ImGuiKey_Keypad6;
      case NLSWIN::KeyValue::KEY_NUMPAD_7:
         return ImGuiKey_Keypad7;
      case NLSWIN::KeyValue::KEY_NUMPAD_8:
         return ImGuiKey_Keypad8;
      case NLSWIN::KeyValue::KEY_NUMPAD_9:
         return ImGuiKey_Keypad9;
      case NLSWIN::KeyValue::KEY_NUMPAD_PERIOD:
         return ImGuiKey_KeypadDecimal;
      case NLSWIN::KeyValue::KEY_NUMPAD_DIVIDE:
         return ImGuiKey_KeypadDivide;
      case NLSWIN::KeyValue::KEY_NUMPAD_MULTIPLY:
         return ImGuiKey_KeypadMultiply;
      case NLSWIN::KeyValue::KEY_NUMPAD_SUBTRACT:
         return ImGuiKey_KeypadSubtract;
      case NLSWIN::KeyValue::KEY_NUMPAD_ADD:
         return ImGuiKey_KeypadAdd;
      case NLSWIN::KeyValue::KEY_NUMPAD_ENTER:
         return ImGuiKey_KeypadEnter;
      default:
         return ImGuiKey_None;
   }
}