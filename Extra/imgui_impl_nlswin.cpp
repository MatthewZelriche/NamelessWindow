#include "imgui_impl_nlswin.hpp"

#include <NamelessWindow/Events/Event.hpp>

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

void ImGui_ImplNLSWin_HandleEvent(NLSWIN::Event ev) {
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplNLSWin_Data *data = (ImGui_ImplNLSWin_Data *)(io.BackendPlatformUserData);
   IM_ASSERT(data != nullptr);

   if (auto event = std::get_if<NLSWIN::MouseMovementEvent>(&ev)) {
      io.AddMousePosEvent(event->newXPos, event->newYPos);
   }
   else if (auto event = std::get_if<NLSWIN::MouseButtonEvent>(&ev)) {
      io.AddMouseButtonEvent(TranslateMouseButton(event->button),
                             event->type == NLSWIN::ButtonPressType::PRESSED);
   } else if (auto event = std::get_if<NLSWIN::MouseScrollEvent>(&ev)) {
      float dir = event->scrollType == NLSWIN::ScrollType::UP ? 1.0f : -1.0f;
      io.AddMouseWheelEvent(0.0f, dir);
   }
}

void ImGui_ImplNLSWin_Shutdown() {
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplNLSWin_Data *data = (ImGui_ImplNLSWin_Data *)(io.BackendPlatformUserData);
   IM_ASSERT(data != nullptr);

   io.BackendPlatformUserData = nullptr;
   io.BackendPlatformName = nullptr;
}