#pragma once

#include "NamelessWindow/Window.hpp"
#include <imgui/imgui.h>
#include <chrono>

IMGUI_IMPL_API bool ImGui_ImplNLSWin_Init(std::weak_ptr<NLSWIN::Window> window);
IMGUI_IMPL_API void ImGui_ImplNLSWin_NewFrame();
IMGUI_IMPL_API void ImGui_ImplNLSWin_HandleEvent(NLSWIN::Event ev);
IMGUI_IMPL_API void ImGui_ImplNLSWin_Shutdown();

constexpr std::chrono::steady_clock::time_point nullLastFrame = std::chrono::steady_clock::time_point::min();

struct ImGui_ImplNLSWin_Data {
   std::weak_ptr<NLSWIN::Window> window;
   std::chrono::steady_clock::time_point lastFrameEnd {nullLastFrame};
};
