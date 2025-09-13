// Everything in here is probably jank
#include <GLFW/glfw3.h>
#include <classes/level.h>
#include <classes/light.h>
#include <classes/material.h>
#include <classes/shader.h>
#include <classes/sprite.h>
#include <classes/texture.h>
#include <engine/debug.h>
#include <engine/localization.h>
#include <engine/devgui.h>
#include <engine/filesystem.h>
#include <engine/render.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <format>
#include <tinyfiledialogs/tinyfiledialogs.h>

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
void MaterialView(Material*& material);
void DrawProperties();
void DrawDebug();
void DrawLevel();
void DrawLocalization();
void DrawMenuFile();
void DrawMenuEdit();
void DrawMenuView();
void DrawMenuEngine();
void DrawMenuHelp();
void DrawMenuBar();

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
      "engine_assets/IBM_Plex_Sans/IBMPlexSans-VariableFont_wdth,wght.ttf", DEVGUI_FONT_SIZE, nullptr,
      imgui_io.Fonts->GetGlyphRangesDefault());
  ImGui_ImplGlfw_InitForOpenGL(render::g_window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  debug::Log(GET_TRACE, "Initialized GUI");
}
#define VIEWPORT_WIDTH 0.7F
void dev::Update() {
  if (!dev::g_hud_enabled) {
    glfwSetInputMode(render::g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  glfwSetInputMode(render::g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                  ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;
  ImGui::Begin("DockSpace_Window", nullptr, window_flags);
  ImGui::PopStyleVar(2);
  ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F));
  ImGui::End();
  static bool first_dock_layout = true;
  if (first_dock_layout) {
    first_dock_layout = false;
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
    ImGuiID dock_main_id = dockspace_id;
    ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, VIEWPORT_WIDTH, nullptr, &dock_main_id);
    ImGui::DockBuilderDockWindow("Viewport", dock_id_left);
    ImGui::DockBuilderDockWindow("Properties", dock_main_id);
    ImGui::DockBuilderDockWindow("Debug", dock_main_id);
    ImGui::DockBuilderDockWindow("Level", dock_main_id);
    ImGui::DockBuilderDockWindow("Localization", dock_main_id);
    ImGui::DockBuilderDockWindow("Renderer", dock_main_id);
    ImGui::DockBuilderFinish(dockspace_id);
  }
  DrawMenuBar();
  DrawProperties();
  DrawDebug();
  DrawLevel();
  DrawLocalization();
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

void DrawLevel() {
  ImGui::Begin("Level");
  if (filesystem::g_level == nullptr) {
    if (ImGui::Button("New Level")) {
      const char* filter_patterns[] = {"*.xml"};
      const char* file = tinyfd_saveFileDialog("Save Level XML", "new_level.xml", 1,
                                               filter_patterns, "Level Files");
      if (file != nullptr) {
        filesystem::g_level = new Level();
        filesystem::g_level->path_ = file;
      }
    }
    if (ImGui::Button("Open Level")) {
      const char* filter_patterns[] = {"*.xml"};
      const char* file =
          tinyfd_openFileDialog("Open Level XML", "", 1, filter_patterns, "Level Files", 0);
      if (file != nullptr) {
        filesystem::g_level = filesystem::serialized::LoadLevel(file);
        g_current_object = nullptr;
      }
    }
  }
  if (filesystem::g_level != nullptr) {
    ImGui::LabelText("Level Path", "%s", filesystem::g_level->path_.c_str());
    if (ImGui::Button("Open Level")) {
      filesystem::FreeLevel(filesystem::g_level);
      const char* filter_patterns[] = {"*.xml"};
      const char* file =
          tinyfd_openFileDialog("Open Level XML", "", 1, filter_patterns, "Level Files", 0);
      if (file != nullptr) {
        filesystem::g_level = filesystem::serialized::LoadLevel(file);
        g_current_object = nullptr;
      }
    }
    if (ImGui::Button("Save Level")) {
      filesystem::serialized::SaveLevel(filesystem::g_level, filesystem::g_level->path_);
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
#define BUFFER_SIZE 128
void DrawLocalization() {
  ImGui::Begin("Localization");
  ImGui::InputText("Language", &localization::g_language);
  ImGui::SameLine();
  if (ImGui::Button("Add")) {
    localization::g_dictionary.insert({"", ""});
  }
  std::vector<std::pair<std::string, std::string>> renamed_entries;
  std::vector<std::string> keys_to_delete;
  if (ImGui::BeginTable("EditableMapTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
    ImGui::TableSetupColumn("Key");
    ImGui::TableSetupColumn("Value");
    ImGui::TableSetupColumn("Actions");
    ImGui::TableHeadersRow();
    int index = 0;
    for (auto& [key, value] : localization::g_dictionary) {
      ImGui::TableNextRow();
      ImGui::PushID(index);
      ImGui::TableSetColumnIndex(0);
      std::array<char, BUFFER_SIZE> key_buffer;
      strncpy(key_buffer.data(), key.c_str(), sizeof(key_buffer));
      key_buffer[sizeof(key_buffer) - 1] = '\0';
      if (ImGui::InputText("##Key", key_buffer.data(), sizeof(key_buffer))) {
      if (key != key_buffer.data() && strlen(key_buffer.data()) > 0) {
        keys_to_delete.push_back(key);
        renamed_entries.emplace_back(key_buffer.data(), value);
        }
      }
      ImGui::TableSetColumnIndex(1);
      std::array<char, BUFFER_SIZE> value_buffer;
      strncpy(value_buffer.data(), value.c_str(), sizeof(value_buffer));
      value_buffer[sizeof(value_buffer) - 1] = '\0';
      if (ImGui::InputText("##Value", value_buffer.data(), sizeof(value_buffer))) {
        localization::g_dictionary[key] = value_buffer.data();
      }
      ImGui::TableSetColumnIndex(2);
      if (ImGui::Button("Remove")) {
        keys_to_delete.push_back(key);
      }
      ImGui::PopID();
      index++;
    }
    for (const auto& key : keys_to_delete) {
      localization::g_dictionary.erase(key);
    }
    for (const auto& [new_key, value] : renamed_entries) {
      localization::g_dictionary[new_key] = value;
    }
    ImGui::EndTable();
  }
  if (ImGui::Button("Load")) {
    const char* filter_patterns[] = {"*.xml"};
    const char* file = tinyfd_openFileDialog("Open Level XML", "", 1, filter_patterns, "Localization Files", 0);
    if (file != nullptr) {
      localization::Load(file);
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    const char* filter_patterns[] = {"*.xml"};
    const char* file = tinyfd_saveFileDialog("Save Level XML", std::string(localization::g_language + ".xml").c_str(), 1, filter_patterns, "Level Files");
    if (file != nullptr) {
      localization::Save(file);
    }
  }
  ImGui::End();
}

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

void DrawMenuFile() {
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("New")) {
      filesystem::FreeLevel(filesystem::g_level);
      filesystem::g_level = new Level();
      g_current_object = nullptr;
    }
    if (ImGui::MenuItem("Open")) {
      const char* filter_patterns[] = {"*.xml"};
      const char* file =
          tinyfd_openFileDialog("Open Level XML", "", 1, filter_patterns, "Level Files", 0);
      if (file != nullptr) {
        filesystem::FreeLevel(filesystem::g_level);
        filesystem::g_level = filesystem::serialized::LoadLevel(file);
        g_current_object = nullptr;
      }
    }
    if (ImGui::MenuItem("Save")) {
      if (filesystem::g_level != nullptr) {
        filesystem::serialized::SaveLevel(filesystem::g_level, filesystem::g_level->path_);
      }
    }
    if (ImGui::MenuItem("Save As")) {
      if (filesystem::g_level != nullptr) {
        const char* filter_patterns[] = {"*.xml"};
        const char* file = tinyfd_saveFileDialog("Save Level XML", "new_level.xml", 1,
                                                 filter_patterns, "Level Files");
        if (file != nullptr) {
          filesystem::serialized::SaveLevel(filesystem::g_level, file);
          filesystem::g_level->path_ = file;
        }
      }
    }
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
