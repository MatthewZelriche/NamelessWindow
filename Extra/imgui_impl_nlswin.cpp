#include "imgui_impl_nlswin.hpp"

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
}

void ImGui_ImplNLSWin_Shutdown() {
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplNLSWin_Data *data = (ImGui_ImplNLSWin_Data *)(io.BackendPlatformUserData);
   IM_ASSERT(data != nullptr);

   io.BackendPlatformUserData = nullptr;
   io.BackendPlatformName = nullptr;
}