
#define GLAD_GL_IMPLEMENTATION
#define NOMINMAX  // Needed so gl.h doesnt start messing up the imgui backend.
#include "demo.hpp"

#include <gl.h>

#include <NamelessWindow/Cursor.hpp>
#include <NamelessWindow/Events/EventBus.hpp>
#include <NamelessWindow/Exceptions.hpp>
#include <NamelessWindow/Keyboard.hpp>
#include <NamelessWindow/RawMouse.hpp>
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
   ImGui::Text("Current Position: %d, %d", window->GetWindowPos().x, window->GetWindowPos().y);
   ImGui::Text("Current Resolution: %d, %d", window->GetWindowWidth(), window->GetWindowHeight());
   ImGui::Separator();
   NLSWIN::Point windowPos = window->GetWindowPos();
   static int pos[2] {windowPos.x, windowPos.y};

   ImGui::InputInt2("##Pos", pos);
   ImGui::SameLine();
   if (ImGui::Button("Set New Position")) {
      window->Reposition(pos[0], pos[1]);
   }
   static int res[2] {window->GetWindowWidth(), window->GetWindowHeight()};
   ImGui::InputInt2("##Res", res);
   ImGui::SameLine();
   if (ImGui::Button("Set New Resolution")) {
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

   std::shared_ptr<NLSWIN::RawMouse> rawMouse = nullptr;

   auto keyboardInfos = NLSWIN::Keyboard::EnumerateKeyboards();
   auto mouseInfos = NLSWIN::RawMouse::EnumeratePointers();

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
   std::string selectedMouse = "";
   int cursorX = 0;
   int cursorY = 0;
   bool m1Down = false;
   bool m2Down = false;
   bool m3Down = false;
   bool m4Down = false;
   bool m5Down = false;
   NLSWIN::ScrollType lastScrollDir = (NLSWIN::ScrollType)-1;
   bool cursorWithinWindow = false;
   NLSWIN::WindowID inWindow = 0;

   bool rm1Down = false;
   bool rm2Down = false;
   bool rm3Down = false;
   bool rm4Down = false;
   bool rm5Down = false;
   NLSWIN::ScrollType rmLastScrollDir = (NLSWIN::ScrollType)-1;

   float bgColor[3];
   bgColor[0] = 0.4f;
   bgColor[1] = 0.4f;
   bgColor[2] = 0.4f;

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

      int deltaX = 0;
      int deltaY = 0;
      while (cursor->HasEvent()) {
         auto evt = cursor->GetNextEvent();
         ImGui_ImplNLSWin_HandleEvent(evt);
         if (auto event = std::get_if<NLSWIN::MouseMovementEvent>(&evt)) {
            cursorX = event->newXPos;
            cursorY = event->newYPos;
         }
         if (auto event = std::get_if<NLSWIN::RawMouseDeltaMovementEvent>(&evt)) {
            deltaX = event->deltaX;
            deltaY = event->deltaY;
         }
         if (auto event = std::get_if<NLSWIN::MouseButtonEvent>(&evt)) {
            switch (event->button) {
               case NLSWIN::ButtonValue::LEFTCLICK: {
                  m1Down = !(bool)event->type;
                  break;
               }
               case NLSWIN::ButtonValue::RIGHTCLICK: {
                  m2Down = !(bool)event->type;
                  break;
               }
               case NLSWIN::ButtonValue::MIDDLECLICK: {
                  m3Down = !(bool)event->type;
                  break;
               }
               case NLSWIN::ButtonValue::MB_4: {
                  m4Down = !(bool)event->type;
                  break;
               }
               case NLSWIN::ButtonValue::MB_5: {
                  m5Down = !(bool)event->type;
                  break;
               }
            }
         }
         if (auto event = std::get_if<NLSWIN::MouseScrollEvent>(&evt)) {
            lastScrollDir = event->scrollType;
         }
         if (auto event = std::get_if<NLSWIN::MouseEnterEvent>(&evt)) {
            cursorWithinWindow = true;
            inWindow = event->sourceWindow;
         }
         if (auto event = std::get_if<NLSWIN::MouseLeaveEvent>(&evt)) {
            cursorWithinWindow = false;
            inWindow = 0;
         }
      }
      int rmDeltaX = 0;
      int rmDeltaY = 0;
      while (rawMouse && rawMouse->HasEvent()) {
         auto evt = rawMouse->GetNextEvent();
         if (auto event = std::get_if<NLSWIN::RawMouseDeltaMovementEvent>(&evt)) {
            rmDeltaX = event->deltaX;
            rmDeltaY = event->deltaY;
         }
         if (auto event = std::get_if<NLSWIN::RawMouseButtonEvent>(&evt)) {
            switch (event->button) {
               case NLSWIN::ButtonValue::LEFTCLICK: {
                  rm1Down = !(bool)event->type;
                  break;
               }
               case NLSWIN::ButtonValue::RIGHTCLICK: {
                  rm2Down = !(bool)event->type;
                  break;
               }
               case NLSWIN::ButtonValue::MIDDLECLICK: {
                  rm3Down = !(bool)event->type;
                  break;
               }
               case NLSWIN::ButtonValue::MB_4: {
                  rm4Down = !(bool)event->type;
                  break;
               }
               case NLSWIN::ButtonValue::MB_5: {
                  rm5Down = !(bool)event->type;
                  break;
               }
            }
         }
         if (auto event = std::get_if<NLSWIN::RawMouseScrollEvent>(&evt)) {
            rmLastScrollDir = event->scrollType;
         }
      }

      while (kb->HasEvent()) {
         auto evt = kb->GetNextEvent();
         ImGui_ImplNLSWin_HandleEvent(evt);
         if (auto event = std::get_if<NLSWIN::KeyEvent>(&evt)) {
            if ((event->code.value == NLSWIN::KeyValue::KEY_S && event->code.modifiers.ctrl &&
                 (event->pressType == NLSWIN::KeyPressType::PRESSED))) {
               cursor->Show();
            }
            if ((event->code.value == NLSWIN::KeyValue::KEY_L && event->code.modifiers.ctrl &&
                 (event->pressType == NLSWIN::KeyPressType::PRESSED))) {
               cursor->BindToWindow(win.get());
            }
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
         ImGui::Separator();
         ImGui::SliderFloat3("BG Color", bgColor, 0, 1);
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
            log.AddLog("%-13s | %-8s | {Ctrl:%d, Shift:%d, Alt:%d, Super:%d, Caps:%d, Num:%d}\n", skip,
                       pressType.c_str(), mods.ctrl, mods.shift, mods.alt, mods.super, mods.capsLock,
                       mods.numLock);
         }
         log.Draw();
         keyEventsThisFrame.clear();
      }

      if (ImGui::CollapsingHeader("Cursor Info: ")) {
         ImGui::Text("Cursor Visibility (Ctrl + S to Undo Hide)");
         if (ImGui::Button("Show", {150, 25})) {
            cursor->Show();
         }
         ImGui::SameLine();
         if (ImGui::Button("Hide", {150, 25})) {
            cursor->Hide();
         }
         ImGui::Separator();
         ImGui::Text("Window confining (Ctrl + L to Confine)");
         if (ImGui::Button("Confine", {150, 25})) {
            cursor->BindToWindow(win.get());
         }
         ImGui::SameLine();
         if (ImGui::Button("Free", {150, 25})) {
            cursor->UnbindFromWindows();
         }
         ImGui::Separator();
         ImGui::Text("XPos: %d, YPos: %d", cursorX, cursorY);
         ImGui::Text("deltaX: %d, deltaY: %d", deltaX, deltaY);
         ImGui::Separator();
         ImGui::Text("Left MouseButton: %d", m1Down);
         ImGui::Text("Right MouseButton: %d", m2Down);
         ImGui::Text("Middle MouseButton: %d", m3Down);
         ImGui::Text("MB4: %d", m4Down);
         ImGui::Text("MB5: %d", m5Down);
         std::string dir;
         switch (lastScrollDir) {
            case NLSWIN::ScrollType::DOWN: {
               dir = "DOWN";
               break;
            }
            case NLSWIN::ScrollType::UP: {
               dir = "UP";
               break;
            }
            case NLSWIN::ScrollType::LEFT: {
               dir = "LEFT";
               break;
            }
            case NLSWIN::ScrollType::RIGHT: {
               dir = "RIGHT";
               break;
            }
            default: {
               dir = "";
               break;
            }
         }
         ImGui::Text("Last Scroll Dir: %s", dir.c_str());
         ImGui::Separator();
         ImGui::BeginDisabled(true);
         ImGui::Checkbox("Is mouse in window?", &cursorWithinWindow);
         ImGui::EndDisabled();
         if (inWindow) {
            ImGui::Text("Cursor inside window %d", inWindow);
         }
      }

      if (ImGui::CollapsingHeader("Raw Mouse: ")) {
         if (ImGui::BeginCombo("Selected Device", selectedMouse.c_str())) {
            ImGui::PushID(100);
            bool selected = false;
            if (ImGui::Selectable("", &selected)) {
               selectedMouse = "";
               rawMouse = nullptr;
            }
            ImGui::PopID();
            for (auto mouseInfo: mouseInfos) {
               if (ImGui::Selectable(mouseInfo.name.c_str(), &selected)) {
                  selectedMouse = mouseInfo.name;
                  rawMouse = NLSWIN::RawMouse::Create(mouseInfo);
               }
            }
            ImGui::EndCombo();
         }
         if (rawMouse) {
            ImGui::Separator();
            ImGui::Text("deltaX: %d, deltaY: %d", rmDeltaX, rmDeltaY);
            ImGui::Separator();
            ImGui::Text("Left MouseButton: %d", rm1Down);
            ImGui::Text("Right MouseButton: %d", rm2Down);
            ImGui::Text("Middle MouseButton: %d", rm3Down);
            ImGui::Text("MB4: %d", rm4Down);
            ImGui::Text("MB5: %d", rm5Down);
            std::string dir;
            switch (rmLastScrollDir) {
               case NLSWIN::ScrollType::DOWN: {
                  dir = "DOWN";
                  break;
               }
               case NLSWIN::ScrollType::UP: {
                  dir = "UP";
                  break;
               }
               case NLSWIN::ScrollType::LEFT: {
                  dir = "LEFT";
                  break;
               }
               case NLSWIN::ScrollType::RIGHT: {
                  dir = "RIGHT";
                  break;
               }
               default: {
                  dir = "";
                  break;
               }
            }
            ImGui::Text("Last Scroll Dir: %s", dir.c_str());
            ImGui::Separator();
         }
      }

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
      glad_glClearColor(bgColor[0], bgColor[1], bgColor[2], 1);
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