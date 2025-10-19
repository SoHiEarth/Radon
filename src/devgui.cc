#include <GLFW/glfw3.h>
#include <classes/component.h>
#include <classes/level.h>
#include <classes/light.h>
#include <classes/material.h>
#include <classes/modelrenderer.h>
#include <classes/object.h>
#include <classes/shader.h>
#include <classes/texture.h>
#include <engine/debug.h>
#include <engine/devgui.h>
#include <engine/io.h>
#include <engine/localization.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <engine/telemetry.h>
#define IMGUI_ENABLE_FREETYPE
#include <classes/mesh.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <algorithm>
#include <array>
#include <filesystem>
#include <format>

class GuiManager {
private:
  Object* current_object_ = nullptr;

public:
  static GuiManager& Get() {
    static GuiManager instance;
    return instance;
  }
  void SetCurrentObject(Object* object) {
    current_object_ = object;
  }
  Object* GetCurrentObject() {
    return current_object_;
  }
};

constexpr ImVec2 kImagePreviewSize = ImVec2(100.0F, 100.0F);
constexpr float kDevguiRoundingMore = 8.0F;
constexpr float kDevguiRoundingLess = 6.0F;
constexpr float kDevguiFontSize = 18.0F;
constexpr float kDefaultShininess = 32.0F;
constexpr float kDockLeftWidth = 0.3F;
constexpr float kDockRightWidth = 0.3F;
constexpr float kDockBottomHeight = 0.4F;
constexpr float kConsoleTypeWidth = 50.0F;
constexpr float kConsoleTracebackWidth = 200.0F;
bool g_disable_hud_after_frame = false;
std::string g_material_diffuse, g_material_specular, g_material_vertex, g_material_fragment;
int g_material_shininess = kDefaultShininess;
std::vector<ConsoleMessage> g_console_messages;

void ModelView(Engine* engine, Model*& model);
void MaterialView(Engine* engine, Material*& material);
void DrawProperties(Engine* engine);
void DrawDebug(Engine* engine);
void DrawLevel(Engine* engine);
void DrawAssets(Engine* engine);
void DrawIO(Engine* engine);
void DrawLocalization(Engine* engine);
void DrawInterfaceStatus();
void DrawConsole();
void DrawTelemetry(Engine* engine);
void DrawMenuFile(Engine* engine);
void DrawMenuEdit(Engine* engine);
void DrawMenuView();
void DrawMenuEngine();
void DrawMenuHelp();
void DrawMenuBar(Engine* engine);

void IGui::IInit() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& imgui_io = ImGui::GetIO();
  imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  imgui_io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = kDevguiRoundingMore;
  style.ChildRounding = kDevguiRoundingMore;
  style.FrameRounding = kDevguiRoundingLess;
  style.PopupRounding = kDevguiRoundingLess;
  style.ScrollbarRounding = kDevguiRoundingLess;
  style.GrabRounding = kDevguiRoundingLess;
  style.TabRounding = kDevguiRoundingLess;

  ImFont* ui_font = imgui_io.Fonts->AddFontFromFileTTF(
    (engine_->GetIO().GetEngineDirectory() + "/IBM_Plex_Sans/IBMPlexSans-VariableFont_wdth,wght.ttf").c_str(),
      kDevguiFontSize, nullptr, imgui_io.Fonts->GetGlyphRangesDefault());
  ImGui_ImplGlfw_InitForOpenGL(engine_->GetRenderer().GetWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 150");
  engine_->GetDebug().SetCallback(AddConsoleMessage);
}

void IGui::IUpdate() {
  if (!engine_->GetGui().GetHud()) {
    glfwSetInputMode(engine_->GetRenderer().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  glfwSetInputMode(engine_->GetRenderer().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
    ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down,
                                                         kDockBottomHeight, nullptr, &dock_main_id);
    ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, kDockLeftWidth,
                                                       nullptr, &dock_main_id);
    ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right,
                                                        kDockRightWidth, nullptr, &dock_main_id);
    ImGui::DockBuilderDockWindow("Viewport", dock_main_id);
    ImGui::DockBuilderDockWindow("Properties", dock_id_right);
    ImGui::DockBuilderDockWindow("Debug", dock_id_right);
    ImGui::DockBuilderDockWindow("Level", dock_id_left);
    ImGui::DockBuilderDockWindow("Localization", dock_main_id);
    ImGui::DockBuilderDockWindow("IRenderer", dock_id_right);
    ImGui::DockBuilderDockWindow("IO", dock_id_bottom);
    ImGui::DockBuilderDockWindow("Assets", dock_id_bottom);
    ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
    ImGui::DockBuilderDockWindow("Telemetry", dock_id_bottom);
    ImGui::DockBuilderDockWindow("Interfaces", dock_id_bottom);
    ImGui::DockBuilderFinish(dockspace_id);
  }
  DrawProperties(engine_);
  DrawDebug(engine_);
  DrawLevel(engine_);
  DrawLocalization(engine_);
  DrawInterfaceStatus();
  DrawConsole();
  DrawIO(engine_);
  DrawAssets(engine_);
  DrawTelemetry(engine_);
  DrawMenuBar(engine_);
}

void IGui::AddConsoleMessage(std::string_view traceback, std::string_view message,
                             std::uint8_t type) {
  ConsoleMessage console_message{.traceback_ = std::string(traceback),
                                 .message_ = std::string(message),
                                 .type_ = ConsoleMessageType(type)};
  g_console_messages.push_back(console_message);
}

void IGui::IRender() {
  auto& gui = engine_->GetGui();
  if (!gui.GetHud()) {
    return;
  }
  ImGui::Render();
  ImGui::EndFrame();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  if (g_disable_hud_after_frame) {
    gui.SetHud(false);
    g_disable_hud_after_frame = false;
  }
}

void IGui::IQuit() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

std::vector<float> g_fps_history;
std::vector<std::map<std::string, std::chrono::milliseconds>> g_timings_history;

void DrawTelemetry(Engine* engine) {
  ImGui::Begin("Telemetry");
  ImGui::SeparatorText("FPS");
  float fps = ImGui::GetIO().Framerate;
  g_fps_history.push_back(fps);
  if (g_fps_history.size() > 100) {
    g_fps_history.erase(g_fps_history.begin());
  }
  if (ImGui::BeginTable("FPSTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0F);
    ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_WidthFixed, 100.0F);
    ImGui::TableSetupColumn("Graph", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("FPS");
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%.1f", fps);
    ImGui::TableSetColumnIndex(2);
    ImGui::PlotLines("##fps_plot", g_fps_history.data(), g_fps_history.size(), 0, nullptr, 0.0F,
                     *std::ranges::max_element(g_fps_history), ImVec2(0, 50));
    ImGui::EndTable();
  }
  ImGui::SeparatorText("Update Timings");
  if (ImGui::BeginTable("TelemetryTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0F);
    ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_WidthFixed, 100.0F);
    ImGui::TableSetupColumn("Graph", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    auto& timings = engine->GetTelemetry().GetTimings();
    g_timings_history.push_back(timings);
    if (g_timings_history.size() > 100) {
      g_timings_history.erase(g_timings_history.begin());
    }
    for (const auto& [label, time] : timings) {
      ImGui::PushID(label.c_str());
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%s", label.c_str());
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%.3f ms", time.count() * 1.0F);
      ImGui::TableSetColumnIndex(2);
      std::array<float, 100> plot_data{};
      int plot_data_count = 0;
      for (const auto& history_entry : g_timings_history) {
        auto it = history_entry.find(label);
        if (it != history_entry.end() && plot_data_count < 100) {
          plot_data[plot_data_count] = it->second.count() * 1.0F;
          plot_data_count++;
        }
      }
      if (plot_data_count > 0) {
        ImGui::PlotLines("##plot", plot_data.data(), plot_data_count, 0, nullptr, 0.0F,
                         *std::max_element(plot_data.begin(), plot_data.begin() + plot_data_count),
                         ImVec2(0, 50));
      }
      ImGui::PopID();
    }
    ImGui::EndTable();
  }

  ImGui::SeparatorText("Initialization Log");
  try {
    auto timings = engine->GetTelemetry().DownloadTimings(kEngineInitName);
    if (ImGui::BeginTable("InitLogsTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
      ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0F);
      ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableHeadersRow();
      for (const auto& [name, time] : timings) {
        ImGui::PushID(&name);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", name.c_str());
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.3f", time.count() * 1.0F);
        ImGui::PopID();
      }
      ImGui::EndTable();
    }
  } catch (...) {
    ImGui::Text("Failed to fetch timings.");
  }
  ImGui::End();
}

void DrawProperties(Engine* engine) {
  ImGui::Begin("Properties");
  if (GuiManager::Get().GetCurrentObject() != nullptr && engine->GetIO().GetLevel() != nullptr) {
    if (ImGui::Button("Remove")) {
      engine->GetIO().GetLevel()->RemoveObject(GuiManager::Get().GetCurrentObject());
      GuiManager::Get().SetCurrentObject(nullptr);
    } else {
      for (const auto& field : GuiManager::Get().GetCurrentObject()->reg_) {
        if (field != nullptr) {
          field->RenderInterface();
        }
      }
      if (ImGui::CollapsingHeader("Transform (Base)")) {
        for (const auto& field : GuiManager::Get().GetCurrentObject()->transform_.reg_) {
          if (field != nullptr) {
            field->RenderInterface();
          }
        }
      }
      int i = 0;
      for (const auto& component : GuiManager::Get().GetCurrentObject()->GetAllComponents()) {
        ImGui::PushID(i);
        if (component != nullptr) {
          if (ImGui::CollapsingHeader(component->GetTypeName())) {
            for (const auto& field : component->reg_) {
              if (field != nullptr) {
                field->RenderInterface();
              }
            }
            if (component->HasModel()) {
              ModelView(engine, component->GetModel());
            }
            if (component->HasMaterial()) {
              MaterialView(engine, component->GetMaterial());
            }
            if (ImGui::Button("...")) {
              ImGui::OpenPopup("Component Utilities");
            }
            if (ImGui::BeginPopup("Component Utilities", 0)) {
              if (ImGui::Button("Remove Component")) {
                GuiManager::Get().GetCurrentObject()->RemoveComponent(component);
                ImGui::CloseCurrentPopup();
              };
              ImGui::EndPopup();
            }
          }
        }
        ImGui::PopID();
        i++;
      }
      ImGui::SeparatorText("Other Info");
      ImGui::Value("Has Initialized", GuiManager::Get().GetCurrentObject()->has_initialized_);
      ImGui::Value("Has Quit", GuiManager::Get().GetCurrentObject()->has_quit_);
    }
  }
  ImGui::End();
}

void DrawDebug(Engine* engine) {
  ImGui::Begin("Debug");
  ImGui::Checkbox("Trace Source File", &engine->GetDebug().Settings().trace_source_file_);
  ImGui::Checkbox("Trace Function Name", &engine->GetDebug().Settings().trace_function_name_);
  ImGui::Checkbox("Trace Line Number", &engine->GetDebug().Settings().trace_line_number_);
  ImGui::End();
}

void DrawConsole() {
  ImGui::Begin("Console");
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), 0,
                    ImGuiWindowFlags_HorizontalScrollbar);
  if (ImGui::BeginTable(
          "ConsoleTable", 3,
          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, kConsoleTypeWidth);
    ImGui::TableSetupColumn("Traceback", ImGuiTableColumnFlags_WidthFixed, kConsoleTracebackWidth);
    ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    for (const auto& message : g_console_messages) {
      ImGui::TableNextRow();
      ImGui::PushID(&message);
      ImGui::TableSetColumnIndex(0);
      switch (message.type_) {
        case ConsoleMessageType::kConsoleMessageTypeInfo:
          ImGui::TextColored(ImVec4(0.0F, 1.0F, 0.0F, 1.0F), "Info");
          break;
        case ConsoleMessageType::kConsoleMessageTypeWarning:
          ImGui::TextColored(ImVec4(1.0F, 1.0F, 0.0F, 1.0F), "Warning");
          break;
        case ConsoleMessageType::kConsoleMessageTypeError:
          ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "Error");
          break;
        default:
          ImGui::Text("Unknown");
          break;
      }
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", message.traceback_.c_str());
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%s", message.message_.c_str());
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
  ImGui::EndChild();
  if (ImGui::Button("Clear")) {
    g_console_messages.clear();
  }
  ImGui::End();
}

static void DevNewLevel(Engine* engine) {
  const std::array<const char*, 128> kFilterPatterns = {"*.xml"};
  const char* file = tinyfd_saveFileDialog("Save Level XML", "new_level.xml", 1,
                                           kFilterPatterns.data(), "Level Files");
  if (file != nullptr) {
    engine->GetIO().SetLevel(std::make_shared<Level>(engine, file));
  }
}

static void DevOpenLevel(Engine* engine) {
  const std::array<const char*, 128> kFilterPatterns = {"*.xml"};
  const char* file =
      tinyfd_openFileDialog("Open Level XML", "", 1, kFilterPatterns.data(), "Level Files", 0);
  if (file != nullptr) {
    engine->GetIO().GetLevel() = engine->GetIO().LoadLevel(file);
    GuiManager::Get().SetCurrentObject(nullptr);
  }
}

void DrawLevel(Engine* engine) {
  ImGui::Begin("Level");
  if (engine->GetIO().GetLevel() == nullptr) {
    if (ImGui::Button("New Level")) {
      DevNewLevel(engine);
    }
    if (ImGui::Button("Open Level")) {
      DevOpenLevel(engine);
    }
    ImGui::End();
    return;
  }
  ImGui::SeparatorText("Scene Objects");
  if (ImGui::BeginTable(
          "AddObjectTable", 1,
          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
    int object_index = 0;
    for (const auto& object : engine->GetIO().GetLevel()->objects_) {
      if (object != nullptr) {
        ImGui::TableNextRow();
        ImGui::PushID(object_index);
        ImGui::TableSetColumnIndex(0);
        if (ImGui::Selectable(std::format("{}", *object->name_).c_str())) {
          GuiManager::Get().SetCurrentObject(object.get());
        }
        ImGui::PopID();
      }
      object_index++;
    }
    ImGui::EndTable();
  }
  ImGui::End();
}

void DrawAssets(Engine* engine) {
  ImGui::Begin("Assets");
  if (ImGui::BeginTable("AssetManagerTable", 3,
          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed, 200.0F);
    ImGui::TableSetupColumn("GUID", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    for (const auto& asset : engine->GetAssetManager().GetAssets()) {
      ImGui::TableNextRow();
      ImGui::PushID(&asset);
      ImGui::TableSetColumnIndex(0);
      switch (asset->GetType()) {
        case AssetType::kAssetTypeLevel:
          ImGui::Text("Level");
          break;
        case AssetType::kAssetTypeModel:
          ImGui::Text("Model");
          break;
        case AssetType::kAssetTypeTexture:
          ImGui::Text("Texture");
          break;
        case AssetType::kAssetTypeShader:
          ImGui::Text("Shader");
          break;
        case AssetType::kAssetTypeMaterial:
          ImGui::Text("Material");
          break;
        case AssetType::kAssetTypeAudio:
          ImGui::Text("Audio");
          break;
        default:
          ImGui::Text("Unknown");
          break;
      }
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", asset->GetPath().data());
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%s", boost::uuids::to_string(asset->GetID()).c_str());
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
  ImGui::End();
}

void DrawIO(Engine* engine) {
  ImGui::Begin("IO");
  for (const auto& [key, value] : engine->GetIO().GetLoadedTextures()) {
    ImGui::Text("Texture Key: %s", key.c_str());
    ImGui::Image(value->id_, kImagePreviewSize);
  }
  ImGui::End();
}

constexpr int kBufferSize = 128;
void DrawLocalization(Engine* engine) {
  auto loc = engine->GetLocalization();
  ImGui::Begin("Localization");
  ImGui::InputText("Language", &loc.GetLanguage());
  ImGui::SameLine();
  if (ImGui::Button("Add")) {
    loc.GetDictionary().insert({"", ""});
  }
  std::vector<std::pair<std::string, std::string>> renamed_entries;
  std::vector<std::string> keys_to_delete;
  if (ImGui::BeginTable(
          "EditableMapTable", 3,
          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
    ImGui::TableSetupColumn("Key");
    ImGui::TableSetupColumn("Value");
    ImGui::TableSetupColumn("Actions");
    ImGui::TableHeadersRow();
    int index = 0;
    for (auto& [key, value] : loc.GetDictionary()) {
      ImGui::TableNextRow();
      ImGui::PushID(index);
      ImGui::TableSetColumnIndex(0);
      std::array<char, kBufferSize> key_buffer{};
      strncpy(key_buffer.data(), key.c_str(), sizeof(key_buffer));
      key_buffer[sizeof(key_buffer) - 1] = '\0';
      if (ImGui::InputText("##Key", key_buffer.data(), sizeof(key_buffer))) {
        if (key != key_buffer.data() && strlen(key_buffer.data()) > 0) {
          keys_to_delete.push_back(key);
          renamed_entries.emplace_back(key_buffer.data(), value);
        }
      }
      ImGui::TableSetColumnIndex(1);
      std::array<char, kBufferSize> value_buffer{};
      strncpy(value_buffer.data(), value.c_str(), sizeof(value_buffer));
      value_buffer[sizeof(value_buffer) - 1] = '\0';
      if (ImGui::InputText("##Value", value_buffer.data(), sizeof(value_buffer))) {
        loc.GetDictionary()[key] = value_buffer.data();
      }
      ImGui::TableSetColumnIndex(2);
      if (ImGui::Button("Remove")) {
        keys_to_delete.push_back(key);
      }
      ImGui::PopID();
      index++;
    }
    for (const auto& key : keys_to_delete) {
      loc.GetDictionary().erase(key);
    }
    for (const auto& [new_key, value] : renamed_entries) {
      loc.GetDictionary()[new_key] = value;
    }
    ImGui::EndTable();
  }
  if (ImGui::Button("Load")) {
    const char* filter_patterns[] = {"*.xml"};
    const char* file = tinyfd_openFileDialog("Open Localization Data", "", 1, filter_patterns,
                                             "Localization Files", 0);
    if (file != nullptr) {
      loc.Load(file);
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Save")) {
    const char* filter_patterns[] = {"*.xml"};
    const char* file = tinyfd_saveFileDialog(
        "Save Localization Data",
        std::string(loc.GetLanguage() + ".xml").c_str(), 1,
        filter_patterns, "Localization Files");
    if (file != nullptr) {
      loc.Save(file);
    }
  }
  ImGui::End();
}

void DrawInterfaceStatus() {
  ImGui::Begin("Interfaces");

  if (ImGui::BeginTable(
          "InterfaceStatusTable", 3,
          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0F);
    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 100.0F);
    ImGui::TableHeadersRow();
    for (const auto& interface : Interface::All()) {
      ImGui::PushID(interface);
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%s", interface->Name());
      ImGui::TableSetColumnIndex(1);
      switch (interface->GetState()) {
        case InterfaceState::kInitialized:
          ImGui::TextColored(ImVec4(0.0F, 1.0F, 0.0F, 1.0F), "Initialized");
          break;
        case InterfaceState::kStopped:
          ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "Stopped");
          break;
      }
      ImGui::TableSetColumnIndex(2);
      if (interface->GetState() == InterfaceState::kInitialized) {
        if (ImGui::Button("Stop")) {
          auto confirm = tinyfd_messageBox(
              "Confirm",
              std::format("Are you sure you want to stop {}?", interface->Name()).c_str(), "yesno",
              "warning", 0);
          if (confirm != 0) {
            interface->Stop();
          }
        }
      } else {
        if (ImGui::Button("Start")) {
          interface->Start();
        }
      }
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
  ImGui::End();
}

void ModelView(Engine* engine, Model*& model) {
  ImGui::SeparatorText("Mesh");
  if (ImGui::BeginTabBar("ModelLoadType")) {
    if (ImGui::BeginTabItem("File")) {
      if (ImGui::Button("Load")) {
        const char* filter_patterns[] = {"*.obj", "*.fbx", "*.gltf", "*.glb"};
        const char* file =
            tinyfd_openFileDialog("Select Mesh File", "", 4, filter_patterns, "Mesh Files", 0);
        if (file != nullptr) {
          model = engine->GetIO().LoadModel(file);
        }
      }
      ImGui::SameLine();
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  if (ImGui::Button("Clear Mesh")) {
    model = nullptr;
  }
  if (model == nullptr) {
    ImGui::TextColored({1, 0, 0, 1}, "No mesh loaded");
    return;
  }
  if (model != nullptr) {
    ImGui::Text("Mesh Preview");
    for (const auto& mesh : model->meshes_) {
      if (!mesh->textures_.empty()) {
        ImGui::Image(mesh->textures_[0]->id_, ImVec2(kImagePreviewSize));
        break;
      }
      ImGui::Text("Vertex Count: %zu", mesh->vertices_.size());
      ImGui::Text("Index Count: %zu", mesh->indices_.size());
    }
    ImGui::Text("Meshes: %zu", model->meshes_.size());
    ImGui::Text("Path: %s", model->GetPath());
  }
}

void MaterialView(Engine* engine, Material*& material) {
  ImGui::SeparatorText("Material");
  if (ImGui::BeginTabBar("MaterialLoadType")) {
    if (ImGui::BeginTabItem("Directory")) {
      if (ImGui::Button("Load")) {
        char* material_path_c = tinyfd_selectFolderDialog("Select Material Directory", "");
        if (material_path_c != nullptr) {
          std::string material_path = std::string(material_path_c);
          try {
            material = engine->GetIO().LoadMaterial(
                material_path + "/diffuse.png", material_path + "/specular.png",
                material_path + "/vert.glsl", material_path + "/frag.glsl", kDefaultShininess);
          } catch (const std::exception& e) {
            tinyfd_messageBox("Error", e.what(), "ok", "error", 1);
            material = nullptr;
          }
          if (material != nullptr) {
            if (material->diffuse_ != nullptr) {
              g_material_diffuse = material->diffuse_->GetPath();
            }
            if (material->specular_ != nullptr) {
              g_material_specular = material->specular_->GetPath();
            }
            g_material_vertex = material->shader_->vertex_path_;
            g_material_fragment = material->shader_->fragment_path_;
          }
        }
      }
      ImGui::EndTabItem();
      ImGui::SameLine();
    }
    if (ImGui::BeginTabItem("Specific")) {
      ImGui::InputText("Material Diffuse", &g_material_diffuse);
      ImGui::InputText("Material Specular", &g_material_specular);
      ImGui::InputText("Material Vertex", &g_material_vertex);
      ImGui::InputText("Material Fragment", &g_material_fragment);
      ImGui::DragInt("Material Shininess", &g_material_shininess);
      if (ImGui::Button("Load")) {
        try {
          material = engine->GetIO().LoadMaterial(g_material_diffuse, g_material_specular,
                                                  g_material_vertex, g_material_fragment,
                                                  g_material_shininess);
        }
        catch (const std::exception& e) {
          tinyfd_messageBox("Error", e.what(), "ok", "error", 1);
          material = nullptr;
        }
      }
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  if (ImGui::Button("Clear Material")) {
    delete material;
    material = nullptr;
  }
  if (material == nullptr) {
    ImGui::TextColored({1, 0, 0, 1}, "No material loaded");
    return;
  }
  if (material != nullptr) {
    ImGui::Text("Diffuse");
    if (material->diffuse_ != nullptr) {
      ImGui::Image(material->diffuse_->id_, ImVec2(kImagePreviewSize));
      ImGui::LabelText("Path", "%s", material->diffuse_->GetPath());
    } else {
      ImGui::TextColored({1, 0, 0, 1}, "Diffuse texture not loaded");
    }
    ImGui::Text("Specular");
    if (material->specular_ != nullptr) {
      ImGui::Image(material->specular_->id_, ImVec2(kImagePreviewSize));
      ImGui::LabelText("Path", "%s", material->specular_->GetPath());
    } else {
      ImGui::TextColored({1, 0, 0, 1}, "Specular texture not loaded");
    }
    ImGui::DragFloat("Shininess", &material->shininess_);
    ImGui::Text("Shader");
    if (material->shader_ == nullptr) {
      ImGui::TextColored({1, 0, 0, 1}, "Shader not loaded");
    } else {
      ImGui::LabelText("Vertex Path", "%s", material->shader_->vertex_path_);
      ImGui::LabelText("Fragment Path", "%s", material->shader_->fragment_path_);
    }
  }
}

void DrawMenuFile(Engine* engine) {
  auto& io = engine->GetIO();
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("New")) {
      DevNewLevel(engine);
    }
    if (ImGui::MenuItem("Open")) {
      DevOpenLevel(engine);
    }
    ImGui::BeginDisabled(io.GetLevel() == nullptr);
    if (ImGui::MenuItem("Save")) {
      if (io.GetLevel() != nullptr) {
        io.SaveLevel(io.GetLevel(), io.GetLevel()->GetPath());
      }
    }
    if (ImGui::MenuItem("Save As")) {
      if (io.GetLevel() != nullptr) {
        const char* filter_patterns[] = {"*.xml"};
        const char* file = tinyfd_saveFileDialog("Save Level XML", "new_level.xml", 1,
                                                 filter_patterns, "Level Files");
        if (file != nullptr) {
          io.SaveLevel(io.GetLevel(), file);
        }
      }
    }
    ImGui::EndDisabled();
    if (ImGui::MenuItem("Exit")) {
      glfwSetWindowShouldClose(engine->GetRenderer().GetWindow(), GLFW_TRUE);
    }
    ImGui::EndMenu();
  }
}

void DrawMenuEdit(Engine* engine) {
  auto& io = engine->GetIO();
  if (ImGui::BeginMenu("Edit")) {
    if (ImGui::BeginMenu("Add")) {
      ImGui::BeginDisabled(io.GetLevel() == nullptr);
      if (ImGui::MenuItem("Empty Object")) {
        if (io.GetLevel() != nullptr) {
          io.GetLevel()->NewObject();
        }
      }
      ImGui::EndDisabled();
      ImGui::BeginDisabled(io.GetLevel() == nullptr ||
                           GuiManager::Get().GetCurrentObject() == nullptr);
      for (const auto& [name, func] : IIO::GetComponentFactory()) {
        if (ImGui::MenuItem(std::format("{}", name).c_str())) {
          if (io.GetLevel() != nullptr &&
              GuiManager::Get().GetCurrentObject() != nullptr) {
            auto component = func();
            GuiManager::Get().GetCurrentObject()->AddComponent(component);
          }
        }
      }
      ImGui::EndDisabled();
      ImGui::EndMenu();
    }
    ImGui::BeginDisabled((GuiManager::Get().GetCurrentObject() == nullptr) ||
                         (io.GetLevel() == nullptr));
    if (ImGui::MenuItem("Remove Selected")) {
      io.GetLevel()->RemoveObject(GuiManager::Get().GetCurrentObject());
    }
    ImGui::EndDisabled();
    ImGui::EndMenu();
  }
}

void DrawMenuView() {
  if (ImGui::BeginMenu("View")) {
    if (ImGui::MenuItem("Toggle HUD", "F1")) {
      g_disable_hud_after_frame = true;
    }
    ImGui::EndMenu();
  }
}

void DrawMenuEngine() {
  if (ImGui::BeginMenu("Engine")) {
    if (ImGui::BeginMenu("Render")) {
      if (ImGui::BeginMenu("Draw Mode")) {
        if (ImGui::MenuItem(
                "Fill", nullptr,
                IRenderer::GetRenderDrawMode() == RenderDrawMode::kFill)) {
          IRenderer::SetRenderDrawMode(RenderDrawMode::kFill);
        }
        if (ImGui::MenuItem(
                "Line", nullptr,
                IRenderer::GetRenderDrawMode() == RenderDrawMode::kLine)) {
          IRenderer::SetRenderDrawMode(RenderDrawMode::kLine);
        }
        if (ImGui::MenuItem(
                "Point", nullptr,
                IRenderer::GetRenderDrawMode() == RenderDrawMode::kPoint)) {
          IRenderer::SetRenderDrawMode(RenderDrawMode::kPoint);
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenu();
  }
}

void DrawMenuHelp() {
  if (ImGui::BeginMenu("Help")) {
    ImGui::EndMenu();
  }
}

void DrawMenuBar(Engine* engine) {
  if (ImGui::BeginMainMenuBar()) {
    DrawMenuFile(engine);
    DrawMenuEdit(engine);
    DrawMenuView();
    DrawMenuEngine();
    DrawMenuHelp();
    ImGui::EndMainMenuBar();
  }
}
