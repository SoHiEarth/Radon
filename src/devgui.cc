// Everything in here is probably jank
#include <engine/devgui.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <engine/global.h>
#include <classes/sprite.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <format>

bool dev::hud_enabled = false;
Object* current_object = nullptr;

void MaterialView(Material* material) {
  if (!material) return;
  if (material->diffuse != nullptr) {
    ImGui::Text("Material - Diffuse");
    ImGui::Image(material->diffuse->id, ImVec2(100, 100));
  }
  if (material->specular != nullptr) {
    ImGui::Text("Material - Specular");
    ImGui::Image(material->specular->id, ImVec2(100, 100));
  }
  ImGui::DragFloat("Material - Shininess", &material->shininess);
}

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
    glfwSetInputMode(GET_WINDOW(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  glfwSetInputMode(GET_WINDOW(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Scene");
  for (int i = 0;i < GET_LEVEL()->objects.size(); i++)
    if (ImGui::Button(std::format("{}", i).c_str()))
      current_object = GET_LEVEL()->objects[i];
  ImGui::End();

  ImGui::Begin("Objects");
  if (ImGui::Button("Add Sprite"))
    GET_LEVEL()->AddObject(new Sprite);
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
      MaterialView(sprite->material);
    }
  }
  ImGui::End();
}

void dev::Render() {
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
