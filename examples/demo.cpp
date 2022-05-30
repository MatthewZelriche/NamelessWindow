
#define GLAD_GL_IMPLEMENTATION
#define NOMINMAX  // Needed so gl.h doesnt start messing up the imgui backend.
#include <gl.h>

#include <NamelessWindow/Cursor.hpp>
#include <NamelessWindow/Events/EventBus.hpp>
#include <NamelessWindow/Exceptions.hpp>
#include <NamelessWindow/Keyboard.hpp>
#include <NamelessWindow/Rendering/GLContext.hpp>
#include <NamelessWindow/Window.hpp>
#include <iostream>

#include "../Extra/imgui_impl_nlswin.hpp"
#include "imgui/backends/imgui_impl_opengl3.h"

int main() {
   NLSWIN::WindowProperties props {0};
   props.horzResolution = 1280;
   props.vertResolution = 768;
   props.yCoordinate = 50;
   props.windowName = "Interactive Demo";

   auto kb = NLSWIN::Keyboard::Create();
   auto win = NLSWIN::Window::Create(props);
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

   int lastHorz = win->GetWindowWidth();
   int lastVert = win->GetWindowHeight();
   bool borderlessChange = false;

   while (!win->RequestedClose()) {
      NLSWIN::EventBus::PollEvents();

      while (win && win->HasEvent()) {
         auto evt = win->GetNextEvent();
         ImGui_ImplNLSWin_HandleEvent(evt);
      }
      while (cursor->HasEvent()) {
         auto evt = cursor->GetNextEvent();
         ImGui_ImplNLSWin_HandleEvent(evt);
      }
      while (kb->HasEvent()) {
         auto evt = kb->GetNextEvent();
         ImGui_ImplNLSWin_HandleEvent(evt);
      }

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplNLSWin_NewFrame();
      ImGui::NewFrame();

      ImGui::SetNextWindowPos(ImVec2 {0, 0});
      ImGui::SetNextWindowSize(ImVec2 {350, (float)win->GetWindowHeight()});
      ImGui::Begin("Interactive NLSWIN Demo", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
      bool resChanged = false;
      if (ImGui::CollapsingHeader("Primary Window Information:", ImGuiTreeNodeFlags_DefaultOpen)) {
         ImGui::Text("Generic Window ID: %d", win->GetGenericID());
         ImGui::Text("Frametime: %fms", io.DeltaTime);
         ImGui::Separator();
         int pos[2];
         NLSWIN::Point windowPos = win->GetWindowPos();

         pos[0] = windowPos.x;
         pos[1] = windowPos.y;
         ImGui::InputInt2("Position", pos);
         if (ImGui::IsItemDeactivatedAfterEdit()) {
            win->Reposition(pos[0], pos[1]);
         }
         int res[2];
         res[0] = win->GetWindowWidth();
         res[1] = win->GetWindowHeight();
         ImGui::InputInt2("Resolution", res);
         resChanged = ImGui::IsItemDeactivatedAfterEdit();
         if (resChanged) {
            try {
               win->Resize(res[0], res[1]);
            } catch (NLSWIN::InvalidVideoModeException e) { ImGui::OpenPopup("Invalid Video Mode"); }
         }
         bool isBorderless = win->IsBorderless();
         if (ImGui::Checkbox("Borderless", &isBorderless)) {
            if (isBorderless) {
               win->EnableBorderless();
            } else {
               win->DisableBorderless();
            }
         }
         int index = (int)win->GetWindowMode();
         ImGui::Combo("Mode", &index, "Fullscreen\0Windowed\0");

         NLSWIN::WindowMode desiredMode = (NLSWIN::WindowMode)index;
         if (desiredMode != win->GetWindowMode()) {
            switch (desiredMode) {
               case NLSWIN::WindowMode::WINDOWED: {
                  win->SetWindowed();
                  break;
               }
               case NLSWIN::WindowMode::FULLSCREEN: {
                  try {
                     win->SetFullscreen();
                  } catch (...) { ImGui::OpenPopup("Invalid Video Mode"); }
                  break;
               }
            }
         }
      }

      ImVec2 clientAreaCenter = ImGui::GetMainViewport()->GetCenter();
      ImGui::SetNextWindowPos(clientAreaCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
      bool alwaysTrue = true;
      if (ImGui::BeginPopupModal("Invalid Video Mode", &alwaysTrue)) {
         ImGui::Text("The requested fullscreen resolution is invalid. \n");
         ImGui::EndPopup();
      }

      if (ImGui::CollapsingHeader("Secondary Window")) {}

      ImGui::End();

      ImGui::Render();
      glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glad_glClearColor(0.4, 0.4, 0.4, 1);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      context->SwapContextBuffers();
   }

   ImGui_ImplNLSWin_Shutdown();
   ImGui_ImplOpenGL3_Shutdown();
   return 0;
}