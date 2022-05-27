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