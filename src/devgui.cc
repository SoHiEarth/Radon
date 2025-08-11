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

void MaterialView(Material* material) {
  if (!material) {
    ImGui::Text("Material is not valid");
    return;
  }
  if (!material->IsValid()) return;
  ImGui::SeparatorText("Material Info");
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
  ImGui::Begin("Scene");
  if (ImGui::Button("Add Sprite")) Engine::level->AddObject(new Sprite);
  if (ImGui::Button("Add Directional Light")) Engine::level->AddObject(new DirectionalLight);
  if (ImGui::Button("Add Point Light")) Engine::level->AddObject(new PointLight);
  if (ImGui::Button("Add Spot Light")) Engine::level->AddObject(new SpotLight);
  ImGui::SeparatorText("Scene Objects");
  int i = 0;
  for (const auto& object : Engine::level->objects) {
    i++;
    if (ImGui::Button(std::format("{} ({})", object->name, i).c_str()))
      current_object = object;
  }
  ImGui::End();

  ImGui::Begin("Inspector");
  if (!current_object) {
    ImGui::Text("Select an Object");
  } else {
    ImGui::DragFloat3("Position", glm::value_ptr(current_object->position), 0.1f);
    ImGui::DragFloat2("Scale", glm::value_ptr(current_object->scale), 0.1f);
    ImGui::DragFloat("Rotation", &current_object->rotation, 0.1f);
    // Temp solution until I get a EditableField<T> system working
    Sprite* sprite = dynamic_cast<Sprite*>(current_object);
    if (sprite != nullptr) {
      ImGui::SeparatorText("Sprite");
      ImGui::InputText("Path", &sprite->path);
      MaterialView(sprite->material);
    }
    DirectionalLight* dLight = dynamic_cast<DirectionalLight*>(current_object);
    if (dLight != nullptr) {
      ImGui::SeparatorText("Directional Light");
      ImGui::DragFloat3("Direction", glm::value_ptr(dLight->direction), 0.1f);
      ImGui::DragFloat3("Ambient", glm::value_ptr(dLight->ambient), 0.1f);
      ImGui::DragFloat3("Diffuse", glm::value_ptr(dLight->diffuse), 0.1f);
      ImGui::DragFloat3("Specular", glm::value_ptr(dLight->specular), 0.1f);
    }
    PointLight* pLight = dynamic_cast<PointLight*>(current_object);
    if (pLight != nullptr) {
      ImGui::SeparatorText("Point Light");
      ImGui::DragFloat3("Ambient", glm::value_ptr(pLight->ambient), 0.1f);
      ImGui::DragFloat3("Diffuse", glm::value_ptr(pLight->diffuse), 0.1f);
      ImGui::DragFloat3("Specular", glm::value_ptr(pLight->specular), 0.1f);
      ImGui::DragFloat("Constant", &pLight->constant, 0.1f);
      ImGui::DragFloat("Linear", &pLight->linear, 0.1f);
      ImGui::DragFloat("Quadratic", &pLight->quadratic, 0.1f);
    }
    SpotLight* sLight = dynamic_cast<SpotLight*>(current_object);
    if (sLight != nullptr) {
      ImGui::SeparatorText("Spot Light");
      ImGui::DragFloat3("Direction", glm::value_ptr(sLight->direction), 0.1f);
      ImGui::DragFloat3("Ambient", glm::value_ptr(sLight->ambient), 0.1f);
      ImGui::DragFloat3("Diffuse", glm::value_ptr(sLight->diffuse), 0.1f);
      ImGui::DragFloat3("Specular", glm::value_ptr(sLight->specular), 0.1f);
      ImGui::DragFloat("Constant", &sLight->constant, 0.1f);
      ImGui::DragFloat("Linear", &sLight->linear, 0.1f);
      ImGui::DragFloat("Quadratic", &sLight->quadratic, 0.1f);
      ImGui::DragFloat("Cutoff", &sLight->cutOff, 0.1f);
      ImGui::DragFloat("Outer Cutoff", &sLight->outerCutOff, 0.1f);
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
