
#define GLAD_GL_IMPLEMENTATION
#include <gl.h>

#include <NamelessWindow/Cursor.hpp>
#include <NamelessWindow/Events/EventBus.hpp>
#include <NamelessWindow/Keyboard.hpp>
#include <NamelessWindow/Rendering/GLContext.hpp>
#include <NamelessWindow/Window.hpp>
#include <iostream>

#include "../Extra/imgui_impl_nlswin.hpp"
#include "imgui/backends/imgui_impl_opengl3.h"

int main() {
   auto kb = NLSWIN::Keyboard::Create();
   auto win = NLSWIN::Window::Create();
   auto cursor = NLSWIN::Cursor::Create();
   auto context = NLSWIN::GLContext::Create(win);
   win->Show();
   kb->SubscribeToWindow(win);

   // Load opengl
   context->MakeContextCurrent();
   int success = gladLoaderLoadGL();
   if (!success) {
      std::cout << "Failed to load OpenGL" << std::endl;
   }

   // Set up ImGui
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO();
   ImGui_ImplNLSWin_Init(win);
   ImGui_ImplOpenGL3_Init();

   while (!win->RequestedClose()) {
      NLSWIN::EventBus::PollEvents();

      while (win && win->HasEvent()) { auto evt = win->GetNextEvent(); }
      while (cursor->HasEvent()) { auto evt = cursor->GetNextEvent(); }

      glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glad_glClearColor(0, 1, 0, 1);
      context->SwapContextBuffers();
   }

   ImGui_ImplNLSWin_Shutdown();
   ImGui_ImplOpenGL3_Shutdown();
   return 0;
}