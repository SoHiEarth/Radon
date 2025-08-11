#include <engine/devgui.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <engine/global.h>
#include <classes/sprite.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
bool dev::hud_enabled = false;
Object* current_object = nullptr;
std::string test_string{"Test"};
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
    Engine::current_level.load()->objects.back()->Init();
  }
  ImGui::End();

  ImGui::Begin("Inspector");
  if (!current_object) {
    ImGui::Text("Select an Object");
  } else {
    ImGui::DragFloat3("Position", glm::value_ptr(current_object->position), 0.1f);
    ImGui::DragFloat2("Scale", glm::value_ptr(current_object->scale), 0.1f);
    ImGui::DragFloat("Rotation", &current_object->rotation, 0.1f);
    Sprite* sprite = dynamic_cast<Sprite*>(current_object);
    if (sprite != nullptr) {
      ImGui::InputText("Path", &sprite->path);
      if (sprite->texture != nullptr) {
        ImGui::Image(sprite->texture->id, ImVec2(100, 100));
      }
    }
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
