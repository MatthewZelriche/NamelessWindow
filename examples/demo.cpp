
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
   props.windowName = "Interactive Demo";
   props.startBorderless = true;

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
         if (auto event = std::get_if<NLSWIN::MouseMovementEvent>(&evt)) {

         }
      }
      while (kb->HasEvent()) {
         auto evt = kb->GetNextEvent();
         ImGui_ImplNLSWin_HandleEvent(evt);
         if (auto event = std::get_if<NLSWIN::CharacterEvent>(&evt)) {
            std::cout << event->character;
         }
      }

      int horz = win->GetWindowWidth();
      int vert = win->GetWindowHeight();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplNLSWin_NewFrame();
      ImGui::NewFrame();
      ImGui::ShowDemoWindow(nullptr);

      ImGui::SetNextWindowPos(ImVec2 {0, 0});
      ImGui::SetNextWindowSize(ImVec2 {350, 700});
      ImGui::Begin("Interactive NLSWIN Demo");
      bool horzChanged = false;
      bool vertChanged = false;
      if (ImGui::CollapsingHeader("Primary Window Information:", ImGuiTreeNodeFlags_DefaultOpen)) {
         ImGui::Text("Frametime: %fms", io.DeltaTime);
         ImGui::InputInt("Vertical Resolution", &horz);
         horzChanged = ImGui::IsItemDeactivatedAfterEdit();
         ImGui::InputInt("Horizontal Resolution", &vert);
         vertChanged = ImGui::IsItemDeactivatedAfterEdit();
         if (horzChanged || vertChanged) {
            try {
               win->Resize(horz, vert);
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

      ImGui::Text("");
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