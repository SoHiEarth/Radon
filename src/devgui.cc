// Everything in here is probably jank
#include <GLFW/glfw3.h>
#include <classes/level.h>
#include <classes/light.h>
#include <classes/material.h>
#include <classes/shader.h>
#include <classes/sprite.h>
#include <classes/texture.h>
#include <engine/devgui.h>
#include <engine/global.h>
#include <engine/filesystem.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define IMAGE_PREVIEW_SIZE 100, 100
bool dev::g_hud_enabled = false;
Object* g_current_object = nullptr;
std::string g_material_path;

void MaterialView(Material* material) {
  if (material == nullptr) {
    ImGui::SeparatorText("Load a Material from disk");
    ImGui::InputText("Path", &g_material_path);
    if (ImGui::Button("Load")) {
      material = new Material(g_material_path);
    }
    return;
  }
  if (!material->IsValid()) {
    return;
  }
  ImGui::SeparatorText("Material");
  ImGui::Text("Diffuse");
  ImGui::Image(material->diffuse_->id_, ImVec2(IMAGE_PREVIEW_SIZE));
  ImGui::Text("Specular");
  ImGui::Image(material->specular_->id_, ImVec2(IMAGE_PREVIEW_SIZE));
  ImGui::DragFloat("Shininess", &material->shininess_);
  ImGui::Text("Shader");
  ImGui::Text("Vert: %s", material->shader_->kVertPath.data());
  ImGui::Text("Frag: %s", material->shader_->kFragPath.data());
}

void dev::Init() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& imgui_io = ImGui::GetIO();
  imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui_ImplGlfw_InitForOpenGL(Engine::g_window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void dev::Update() {
  if (!dev::g_hud_enabled) {
    glfwSetInputMode(Engine::g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  glfwSetInputMode(Engine::g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Scene Management");
  if (ImGui::Button("Save Level")) {
    f::SaveLevel(Engine::g_level, "test.xml");
  }
  if (ImGui::Button("Add Sprite")) {
    Engine::g_level->AddObject(new Sprite);
  }
  if (ImGui::Button("Add Directional Light")) {
    Engine::g_level->AddObject(new DirectionalLight);
  }
  if (ImGui::Button("Add Point Light")) {
    Engine::g_level->AddObject(new PointLight);
  }
  if (ImGui::Button("Add Spot Light")) {
    Engine::g_level->AddObject(new SpotLight);
  }
  ImGui::SeparatorText("Scene Objects");
  for (int i = 0; i < Engine::g_level->objects_.size(); i++) {
    if (ImGui::Button(std::format("{}###{}", *Engine::g_level->objects_[i]->name_, i).c_str())) {
      g_current_object = Engine::g_level->objects_[i];
    }
  }
  ImGui::End();

  ImGui::Begin("Properties");
  if (g_current_object == nullptr) {
    ImGui::Text("Select an Object");
  } else {
    for (const auto& field : g_current_object->reg_) {
      if (field != nullptr) {
        field->RenderInterface();
      }
    }
    MaterialView(g_current_object->material_);
  }
  ImGui::End();
}

void dev::Render() {
  if (!dev::g_hud_enabled) {
    return;
  }
  ImGui::Render();
  ImGui::EndFrame();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void dev::Quit() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
