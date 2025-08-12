// Everything in here is probably jank
#include <engine/devgui.h>
#include <engine/global.h>
#include <classes/sprite.h>
#include <classes/light.h>
#include <classes/texture.h>
#include <classes/shader.h>
#include <classes/level.h>
#include <classes/material.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <format>

bool dev::hud_enabled = false;
Object* current_object = nullptr;
std::string material_path = "";

void MaterialView(Material* material) {
  if (!material) {
    ImGui::SeparatorText("Load a Material from disk");
    ImGui::InputText("Path", &material_path);
    if (ImGui::Button("Load"))
      material = new Material(material_path);
    return;
  }
  if (!material->IsValid()) return;
  ImGui::SeparatorText("Material");
  ImGui::Text("Diffuse");
  ImGui::Image(material->diffuse->id, ImVec2(100, 100));
  ImGui::Text("Specular");
  ImGui::Image(material->specular->id, ImVec2(100, 100));
  ImGui::DragFloat("Shininess", &material->shininess);
  ImGui::Text("Shader");
  ImGui::Text("Vert: %s", material->shader->vert_path.data());
  ImGui::Text("Frag: %s", material->shader->frag_path.data());
}

void dev::Init() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  ImGui_ImplGlfw_InitForOpenGL(Engine::window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void dev::Update() {
  if (!dev::hud_enabled) {
    glfwSetInputMode(Engine::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  glfwSetInputMode(Engine::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Scene Management");
  if (ImGui::Button("Add Sprite")) Engine::level->AddObject(new Sprite);
  if (ImGui::Button("Add Directional Light")) Engine::level->AddObject(new DirectionalLight);
  if (ImGui::Button("Add Point Light")) Engine::level->AddObject(new PointLight);
  if (ImGui::Button("Add Spot Light")) Engine::level->AddObject(new SpotLight);
  ImGui::SeparatorText("Scene Objects");
  int i = 0;
  for (const auto& object : Engine::level->objects) {
    i++;
    if (ImGui::Button(std::format("{}###{}", object->name->c_str(), i).c_str()))
      current_object = object;
  }
  ImGui::End();

  ImGui::Begin("Properties");
  if (!current_object) {
    ImGui::Text("Select an Object");
  } else {
    for (const auto& field : current_object->reg)
      if (field != nullptr) field->RenderInterface();
    MaterialView(current_object->material);
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
