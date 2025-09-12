// Everything in here is probably jank
#include <GLFW/glfw3.h>
#include <classes/level.h>
#include <classes/light.h>
#include <classes/material.h>
#include <classes/shader.h>
#include <classes/sprite.h>
#include <classes/texture.h>
#include <engine/debug.h>
#include <engine/devgui.h>
#include <engine/filesystem.h>
#include <engine/render.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>
#include <format>
#define IMAGE_PREVIEW_SIZE 100, 100
#define DEFAULT_SHININESS 32.0F
bool dev::g_hud_enabled = false;
Object* g_current_object = nullptr;
std::string g_material_path;

void MaterialView(Material*& material) {
  ImGui::SeparatorText("Material");
  ImGui::InputText("Material Path", &g_material_path);
  if (ImGui::Button("Load")) {
    filesystem::FreeMaterial(material);
    material = filesystem::LoadMaterial(
        g_material_path + "/diffuse.png", g_material_path + "/specular.png",
        g_material_path + "/vert.glsl", g_material_path + "/frag.glsl", DEFAULT_SHININESS);
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear")) {
    filesystem::FreeMaterial(material);
    material = nullptr;
  }
  if (material == nullptr) {
    ImGui::TextColored({1, 0, 0, 1}, "No material loaded");
    return;
  }
  if (material != nullptr) {
    ImGui::Text("Diffuse");
    if (material->diffuse_ != nullptr) {
      ImGui::Image(material->diffuse_->id_, ImVec2(IMAGE_PREVIEW_SIZE));
      ImGui::Text("Path: %s", material->diffuse_->path_.data());
    } else {
      ImGui::TextColored({1, 0, 0, 1}, "Diffuse texture not loaded");
    }
    ImGui::Text("Specular");
    if (material->specular_ != nullptr) {
      ImGui::Image(material->specular_->id_, ImVec2(IMAGE_PREVIEW_SIZE));
      ImGui::Text("Path: %s", material->specular_->path_.data());
    } else {
      ImGui::TextColored({1, 0, 0, 1}, "Specular texture not loaded");
    }
    ImGui::DragFloat("Shininess", &material->shininess_);
    ImGui::Text("Shader");
    if (material->shader_ == nullptr) {
      ImGui::TextColored({1, 0, 0, 1}, "Shader not loaded");
    } else {
      ImGui::Text("Vert: %s", material->shader_->vertex_path_.data());
      ImGui::Text("Frag: %s", material->shader_->fragment_path_.data());
    }
  }
}

void dev::Init() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& imgui_io = ImGui::GetIO();
  imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui_ImplGlfw_InitForOpenGL(render::g_window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  ImGui::StyleColorsDark();
  debug::Log(GET_TRACE, "Initialized GUI");
}

std::string g_new_level_path = "Untitled Level.xml";
void dev::Update() {
  if (!dev::g_hud_enabled) {
    glfwSetInputMode(render::g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  glfwSetInputMode(render::g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Properties");
  if (g_current_object != nullptr && filesystem::g_level != nullptr) {
    if (ImGui::Button("Remove")) {
      filesystem::g_level->RemoveObject(g_current_object);
      g_current_object = nullptr;
    } else {
      for (const auto& field : g_current_object->reg_) {
        if (field != nullptr) {
          field->RenderInterface();
        }
      }
      MaterialView(g_current_object->material_);
    }
  }
  ImGui::End();
  ImGui::Begin("Debug");
  if (debug::g_debug_settings.kTraceSupported) {
    ImGui::Checkbox("Trace Source File", &debug::g_debug_settings.trace_source_file_);
    ImGui::Checkbox("Trace Function Name", &debug::g_debug_settings.trace_function_name_);
    ImGui::Checkbox("Trace Line Number", &debug::g_debug_settings.trace_line_number_);
  } else {
    ImGui::Text(
        "Application does not support tracebacks; build platform does not support <stacktrace>");
  }
  ImGui::End();
  ImGui::Begin("Level Management");
  if (filesystem::g_level == nullptr) {
    ImGui::InputText("Level Path", &g_new_level_path);
    if (ImGui::Button("Load Level")) {
      if (g_new_level_path.empty()) {
        g_new_level_path = "Untitled Level.xml";
      }
      filesystem::g_level = new Level();
      filesystem::g_level->path_ = g_new_level_path;
      g_current_object = nullptr;
    }
  }
  if (filesystem::g_level != nullptr) {
    ImGui::InputText("Level Path", &g_new_level_path);
    if (ImGui::Button("Load Level")) {
      filesystem::FreeLevel(filesystem::g_level);
      filesystem::g_level = filesystem::serialized::LoadLevel(g_new_level_path);
      g_new_level_path = filesystem::g_level->path_;
      g_current_object = nullptr;
    }
    if (ImGui::Button("Save Level")) {
      filesystem::serialized::SaveLevel(filesystem::g_level, filesystem::g_level->path_);
    }
    if (ImGui::Button("Add Sprite")) {
      filesystem::g_level->AddObject(new Sprite, "Sprite");
    }
    if (ImGui::Button("Add Directional Light")) {
      filesystem::g_level->AddObject(new DirectionalLight, "Directional Light");
    }
    if (ImGui::Button("Add Point Light")) {
      filesystem::g_level->AddObject(new PointLight, "Point Light");
    }
    if (ImGui::Button("Add Spot Light")) {
      filesystem::g_level->AddObject(new SpotLight, "Spot Light");
    }
    ImGui::SeparatorText("Scene Objects");
    if (!filesystem::g_level->objects_.empty()) {
      for (int i = 0; i < filesystem::g_level->objects_.size(); i++) {
        if (ImGui::Button(
                std::format("{}##{}", *filesystem::g_level->objects_[i]->name_, i).c_str())) {
          g_current_object = filesystem::g_level->objects_[i];
        }
      }
    }
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
