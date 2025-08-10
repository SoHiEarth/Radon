#include <engine/devgui.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <engine/global.h>
#include <classes/sprite.h>

bool dev::hud_enabled = false;

Object* current_object = nullptr;

void dev::Init() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  ImGui_ImplGlfw_InitForOpenGL(Engine::window.load(), true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void dev::Update() {
  if (!dev::hud_enabled) {
    glfwSetInputMode(Engine::window.load(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  glfwSetInputMode(Engine::window.load(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::ShowDemoWindow();
  ImGui::Begin("Scene");
  for (int i = 0;i < Engine::current_level.load()->objects.size(); i++) {
    if (ImGui::Button(std::to_string(i).c_str())) {
      current_object = Engine::current_level.load()->objects[i];
    }
  }
  ImGui::End();

  ImGui::Begin("Objects");
  if (ImGui::Button("Add Sprite")) {
    Engine::current_level.load()->objects.push_back(new Sprite());
  }
  ImGui::End();

  ImGui::Begin("Inspector");
  if (!current_object) {
    ImGui::Text("Select an object");
  } else {
    ImGui::Text("Sprite Selected");
  }
  ImGui::End();
}

void dev::LateUpdate() {
  if (!dev::hud_enabled) return;
  ImGui::Render();
  ImGui::EndFrame();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void dev::Quit() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
