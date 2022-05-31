
#define GLAD_GL_IMPLEMENTATION
#define NOMINMAX  // Needed so gl.h doesnt start messing up the imgui backend.
#include "demo.hpp"

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

void WindowHeader(std::shared_ptr<NLSWIN::Window> window, int id) {
   ImGui::PushID(id);
   ImGuiIO &io = ImGui::GetIO();
   ImGui::Text("Generic Window ID: %d", window->GetGenericID());
   ImGui::Text("Frametime: %fms", io.DeltaTime);
   ImGui::Separator();
   int pos[2];
   NLSWIN::Point windowPos = window->GetWindowPos();

   pos[0] = windowPos.x;
   pos[1] = windowPos.y;
   ImGui::InputInt2("Position", pos);
   if (ImGui::IsItemDeactivatedAfterEdit()) {
      window->Reposition(pos[0], pos[1]);
   }
   int res[2];
   res[0] = window->GetWindowWidth();
   res[1] = window->GetWindowHeight();
   ImGui::InputInt2("Resolution", res);
   bool resChanged = ImGui::IsItemDeactivatedAfterEdit();
   if (resChanged) {
      try {
         window->Resize(res[0], res[1]);
      } catch (NLSWIN::InvalidVideoModeException e) { ImGui::OpenPopup("Invalid Video Mode"); }
   }
   bool isBorderless = window->IsBorderless();
   if (ImGui::Checkbox("Borderless", &isBorderless)) {
      if (isBorderless) {
         window->EnableBorderless();
      } else {
         window->DisableBorderless();
      }
   }
   int index = (int)window->GetWindowMode();
   ImGui::Combo("Mode", &index, "Fullscreen\0Windowed\0");

   NLSWIN::WindowMode desiredMode = (NLSWIN::WindowMode)index;
   if (desiredMode != window->GetWindowMode()) {
      switch (desiredMode) {
         case NLSWIN::WindowMode::WINDOWED: {
            window->SetWindowed();
            break;
         }
         case NLSWIN::WindowMode::FULLSCREEN: {
            try {
               window->SetFullscreen();
            } catch (...) { ImGui::OpenPopup("Invalid Video Mode"); }
            break;
         }
      }
   }
   ImGui::PopID();
}

int main() {
   NLSWIN::WindowProperties props {0};
   props.horzResolution = 1280;
   props.vertResolution = 768;
   props.yCoordinate = 50;
   props.windowName = "Interactive Demo";

   auto win = NLSWIN::Window::Create(props);
   auto kb = NLSWIN::Keyboard::Create();
   auto cursor = NLSWIN::Cursor::Create();
   auto context = NLSWIN::GLContext::Create(win);
   win->Show();
   kb->SubscribeToWindow(win);

   auto keyboardInfos = NLSWIN::Keyboard::EnumerateKeyboards();

   std::shared_ptr<NLSWIN::Window> win2 = nullptr;
   std::shared_ptr<NLSWIN::GLContext> context2 = nullptr;

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
   std::string selectedKB = "Master";

   while (!win->RequestedClose()) {
      std::vector<NLSWIN::KeyEvent> keyEventsThisFrame;
      NLSWIN::EventBus::PollEvents();

      if (win2 && win2->RequestedClose()) {
         win2 = nullptr;
      }

      while (win2 && win2->HasEvent()) {
         auto evt = win2->GetNextEvent();
         ImGui_ImplNLSWin_HandleEvent(evt);
      }

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
         if (auto event = std::get_if<NLSWIN::KeyEvent>(&evt)) {
            keyEventsThisFrame.push_back(*event);
         }
      }

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplNLSWin_NewFrame();
      ImGui::NewFrame();

      ImGui::SetNextWindowPos(ImVec2 {0, 0});
      ImGui::SetNextWindowSize(ImVec2 {450, (float)win->GetWindowHeight()});
      ImGui::Begin("Interactive NLSWIN Demo", nullptr,
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

      if (ImGui::CollapsingHeader("Primary Window: ", ImGuiTreeNodeFlags_DefaultOpen)) {
         WindowHeader(win, 1);
      }

      if (ImGui::CollapsingHeader("Secondary Window: ")) {
         if (ImGui::Button("Create", {150, 25})) {
            if (win2) {
               ImGui::OpenPopup("Already Created");
            } else {
               NLSWIN::WindowProperties props;
               props.yCoordinate = 50;
               props.windowName = "Secondary window";
               win2 = NLSWIN::Window::Create(props);
               win2->Show();
               context2 = NLSWIN::GLContext::Create(win2);
            }
         }
         ImGui::SameLine();
         if (ImGui::Button("Destroy", {150, 25})) {
            if (win2) {
               win2 = nullptr;
            } else {
               ImGui::OpenPopup("Doesn't Exist");
            }
         }

         if (win2) {
            WindowHeader(win2, 2);
            if (ImGui::Button("Focus", {175, 25})) {
               win2->Focus();
            }
         }
      }

      if (ImGui::CollapsingHeader("Keyboard Input: ")) {
         if (ImGui::BeginCombo("Selected Device", selectedKB.c_str())) {
            bool selected = false;
            if (ImGui::Selectable("Master", &selected)) {
               selectedKB = "Master";
               kb = NLSWIN::Keyboard::Create();
               kb->SubscribeToWindow(win);
            }
            for (auto kbInfo: keyboardInfos) {
               if (ImGui::Selectable(kbInfo.name.c_str(), &selected)) {
                  selectedKB = kbInfo.name;
                  kb = NLSWIN::Keyboard::Create(kbInfo);
                  kb->SubscribeToWindow(win);
               }
            }
            ImGui::EndCombo();
         }
         char buf[1024] {0};
         ImGui::InputTextMultiline("Text Input", buf, 1024, {0, 70});
         ImGui::Text("Key events: ");
         ImGui::Separator();
         static ExampleAppLog log;
         std::string pressType;
         for (int i = 0; i < keyEventsThisFrame.size(); i++) {
            NLSWIN::KeyEvent evt = keyEventsThisFrame[i];
            switch (evt.pressType) {
               case NLSWIN::KeyPressType::PRESSED: {
                  pressType = "Pressed";
                  break;
               }
               case NLSWIN::KeyPressType::REPEAT: {
                  pressType = "Repeat";
                  break;
               }
               case NLSWIN::KeyPressType::RELEASED: {
                  pressType = "Released";
                  break;
               }
               default:
                  pressType = "Unknown";
                  break;
            }
            NLSWIN::KeyModifiers mods = evt.code.modifiers;
            const char *skip = &(evt.keyName.c_str()[4]);
            log.AddLog("%-13s | %-8s | {Ctrl:%d, Shift:%d, Alt:%d, Super:%d, Caps:%d, Num:%d}\n",
                       skip, pressType.c_str(), mods.ctrl, mods.shift, mods.alt, mods.super,
                       mods.capsLock, mods.numLock);
         }
         log.Draw();
         keyEventsThisFrame.clear();
      }

      if (ImGui::CollapsingHeader("Cursor Info: ")) {}

      ImVec2 clientAreaCenter = ImGui::GetMainViewport()->GetCenter();
      ImGui::SetNextWindowPos(clientAreaCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
      bool alwaysTrue = true;
      if (ImGui::BeginPopupModal("Invalid Video Mode", &alwaysTrue)) {
         ImGui::Text("The requested fullscreen resolution is invalid. \n");
         ImGui::EndPopup();
      }
      if (ImGui::BeginPopupModal("Already Created", &alwaysTrue)) {
         ImGui::Text("Cannot instantiate second window more than once. \n");
         ImGui::EndPopup();
      }

      if (ImGui::BeginPopupModal("Doesn't Exist", &alwaysTrue)) {
         ImGui::Text("Cannot destroy secondary window as it does not exist. \n");
         ImGui::EndPopup();
      }

      ImGui::End();

      ImGui::Render();
      glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glad_glClearColor(0.4, 0.4, 0.4, 1);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      context->SwapContextBuffers();

      if (win2) {
         context2->MakeContextCurrent();
         glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glad_glClearColor(0.39, 0.584, 0.929, 1);
         context2->SwapContextBuffers();
         context->MakeContextCurrent();
      }
   }

   ImGui_ImplNLSWin_Shutdown();
   ImGui_ImplOpenGL3_Shutdown();
   return 0;
}