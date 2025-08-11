// Everything in here is probably jank
#include <engine/devgui.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <engine/global.h>
#include <classes/sprite.h>
#include <classes/light.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <format>
#include <classes/texture.h>

bool dev::hud_enabled = false;
Object* current_object = nullptr;

void MaterialView(Material* material) {
  if (!material) {
    ImGui::Text("Material is not valid");
    return;
  }
  ImGui::SeparatorText("Material Info");
  if (material->diffuse != nullptr) {
    ImGui::Text("Diffuse");
    ImGui::Image(material->diffuse->id, ImVec2(100, 100));
  }
  if (material->specular != nullptr) {
    ImGui::Text("Specular");
    ImGui::Image(material->specular->id, ImVec2(100, 100));
  }
  ImGui::DragFloat("Material - Shininess", &material->shininess);
  if (material->shader != nullptr) {
    ImGui::Text("Shader");
    ImGui::Text("Vert: %s", material->shader->vert_path.data());
    ImGui::Text("Frag: %s", material->shader->frag_path.data());
  }
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
  if (ImGui::Button("Add Directional Light"))
    GET_LEVEL()->AddObject(new DirectionalLight);
  if (ImGui::Button("Add Point Light"))
    GET_LEVEL()->AddObject(new PointLight);
  if (ImGui::Button("Add Spot Light"))
    GET_LEVEL()->AddObject(new SpotLight);
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
