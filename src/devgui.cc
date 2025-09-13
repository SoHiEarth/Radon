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
#define DEVGUI_ROUNDING_MORE 8.0F
#define DEVGUI_ROUNDING_LESS 6.0F
#define DEVGUI_FONT_SIZE 18.0F
#define DEFAULT_SHININESS 32.0F
bool dev::g_hud_enabled = false;
Object* g_current_object = nullptr;
std::string g_material_path;

std::string g_material_diffuse, g_material_specular, g_material_vertex, g_material_fragment;
int g_material_shininess = DEFAULT_SHININESS;

void MaterialView(Material*& material) {
  ImGui::SeparatorText("Material");
  if (ImGui::BeginTabBar("LoadType")) {
    if (ImGui::BeginTabItem("Directory")) {
      ImGui::InputText("Material Path", &g_material_path);
      if (ImGui::Button("Load")) {
        filesystem::FreeMaterial(material);
        material = filesystem::LoadMaterial(
            g_material_path + "/diffuse.png", g_material_path + "/specular.png",
            g_material_path + "/vert.glsl", g_material_path + "/frag.glsl", DEFAULT_SHININESS);
        g_material_diffuse = material->diffuse_->path_;
        g_material_specular = material->specular_->path_;
        g_material_vertex = material->shader_->vertex_path_;
        g_material_fragment = material->shader_->fragment_path_;
      }
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Specific")) {
      ImGui::InputText("Material Diffuse", &g_material_diffuse);
      ImGui::InputText("Material Specular", &g_material_specular);
      ImGui::InputText("Material Vertex", &g_material_vertex);
      ImGui::InputText("Material Fragment", &g_material_fragment);
      ImGui::DragInt("Material Shininess", &g_material_shininess);
      if (ImGui::Button("Load")) {
        filesystem::FreeMaterial(material);
        material =
            filesystem::LoadMaterial(g_material_diffuse, g_material_specular, g_material_vertex,
                                     g_material_fragment, g_material_shininess);
      }
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
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
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = DEVGUI_ROUNDING_MORE;
  style.ChildRounding = DEVGUI_ROUNDING_MORE;
  style.FrameRounding = DEVGUI_ROUNDING_LESS;
  style.PopupRounding = DEVGUI_ROUNDING_LESS;
  style.ScrollbarRounding = DEVGUI_ROUNDING_LESS;
  style.GrabRounding = DEVGUI_ROUNDING_LESS;
  style.TabRounding = DEVGUI_ROUNDING_LESS;
  ImFont* ui_font = imgui_io.Fonts->AddFontFromFileTTF(
      "IBM_Plex_Sans/IBMPlexSans-VariableFont_wdth,wght.ttf", DEVGUI_FONT_SIZE, nullptr,
      imgui_io.Fonts->GetGlyphRangesDefault());
  ImGui_ImplGlfw_InitForOpenGL(render::g_window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  debug::Log(GET_TRACE, "Initialized GUI");
}

void DrawProperties() {
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
      ImGui::SeparatorText("Other Info");
      ImGui::Text("Type: %s", g_current_object->GetTypeName().data());
      ImGui::Text("Has Initialized: %s", g_current_object->has_initialized_ ? "Yes" : "No");
      ImGui::Text("Has Quit: %s", g_current_object->has_quit_ ? "Yes" : "No");
    }
  }
  ImGui::End();
}

void DrawDebug() {
  ImGui::Begin("Debug");
  if (debug::g_debug_settings.kTraceSupported) {
    ImGui::Checkbox("Trace Source File", &debug::g_debug_settings.trace_source_file_);
    ImGui::Checkbox("Trace Function Name", &debug::g_debug_settings.trace_function_name_);
    ImGui::Checkbox("Trace Line Number", &debug::g_debug_settings.trace_line_number_);
  } else {
    ImGui::Text("Build platform did not support <stacktrace>");
  }
  ImGui::End();
}

std::string g_new_level_path = "Untitled Level.xml";
void DrawLevel() {
  ImGui::Begin("Level");
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
    for (const auto& [name, func] : filesystem::g_object_factory) {
      if (ImGui::Button(std::format("Add {}", name).c_str())) {
        filesystem::g_level->AddObject(func(), name);
      }
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

void DrawMenuFile() {
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("Exit")) {
      glfwSetWindowShouldClose(render::g_window, GLFW_TRUE);
    }
    ImGui::EndMenu();
  }
}

void DrawMenuEdit() {
  if (ImGui::BeginMenu("Edit")) {
    if (ImGui::BeginMenu("Add")) {
      ImGui::BeginDisabled(filesystem::g_level == nullptr);
      for (const auto& [name, func] : filesystem::g_object_factory) {
        if (ImGui::MenuItem(std::format("{}", name).c_str())) {
          filesystem::g_level->AddObject(func(), name);
        }
      }
      ImGui::EndDisabled();
      ImGui::EndMenu();
    }
    ImGui::BeginDisabled((g_current_object == nullptr) || (filesystem::g_level == nullptr));
    if (ImGui::MenuItem("Remove Selected")) {
      filesystem::g_level->RemoveObject(g_current_object);
    }
    ImGui::EndDisabled();
    ImGui::EndMenu();
  }
}

void DrawMenuView() {
  if (ImGui::BeginMenu("View")) {
    ImGui::EndMenu();
  }
}

void DrawMenuEngine() {
  if (ImGui::BeginMenu("Engine")) {
    ImGui::EndMenu();
  }
}

void DrawMenuHelp() {
  if (ImGui::BeginMenu("Help")) {
    ImGui::EndMenu();
  }
}

void DrawMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    DrawMenuFile();
    DrawMenuEdit();
    DrawMenuView();
    DrawMenuEngine();
    DrawMenuHelp();
    ImGui::EndMainMenuBar();
  }
}

void dev::Update() {
  if (!dev::g_hud_enabled) {
    glfwSetInputMode(render::g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  glfwSetInputMode(render::g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  DrawMenuBar();
  DrawProperties();
  DrawDebug();
  DrawLevel();
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
