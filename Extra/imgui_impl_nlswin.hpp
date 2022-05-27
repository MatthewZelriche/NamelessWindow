#pragma once

#include "NamelessWindow/Window.hpp"
#include <imgui/imgui.h>

IMGUI_IMPL_API bool ImGui_ImplNLSWin_Init(std::weak_ptr<NLSWIN::Window> window);
IMGUI_IMPL_API void ImGui_ImplNLSWin_NewFrame();
IMGUI_IMPL_API void ImGui_ImplNLSWin_HandleEvent(NLSWIN::Event ev);
IMGUI_IMPL_API void ImGui_ImplNLSWin_Shutdown();

struct ImGui_ImplNLSWin_Data {
   std::weak_ptr<NLSWIN::Window> window;
};
