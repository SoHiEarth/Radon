//
#include <glad/glad.h>
//

#include <GLFW/glfw3.h>
#include <classes/audiosource.h>
#include <classes/level.h>
#include <classes/light.h>
#include <classes/modelrenderer.h>
#include <classes/object.h>
#include <classes/physicsobject.h>
#include <classes/shader.h>
#include <classes/texture.h>
#include <classes/transform.h>
#include <engine/debug.h>
#include <engine/io.h>
#include <fmt/core.h>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <algorithm>
#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <pugixml.hpp>
#include <sstream>
#include <unordered_map>

constexpr const char* kMaterialKeyName = "Material";
constexpr const char* kMaterialDiffuseKeyName = "diffuse";
constexpr const char* kMaterialSpecularKeyName = "specular";
constexpr const char* kMaterialVertexKeyName = "vs";
constexpr const char* kMaterialFragmentKeyName = "fs";
constexpr const char* kMaterialShininessKeyName = "shininess";
constexpr float kMaterialShininessDefaultValue = 32.0F;
#define OBJECT_FACTORY_KEY(name)                \
  {                                             \
    #name, {                                    \
      []() { return std::make_unique<name>(); } \
    }                                           \
  }

enum : std::uint16_t { kLogSize = 512 };
bool IIO::CheckFile(std::string_view path) {
  return std::filesystem::exists(path);
}

void IIO::IInit() {
  auto& debug = engine_->GetDebug();
  if (!CheckFile("engine_assets")) {
    tinyfd_messageBox("Engine Directory Not Found",
                      "Please select the engine directory to continue.", "ok", "info", 1);
    char* engine_path = tinyfd_selectFolderDialog("Select the engine directory",
                                                  std::filesystem::current_path().string().c_str());
    if (engine_path != nullptr) {
      g_engine_directory_ = std::string(engine_path);
      bool copy_assets =
          tinyfd_messageBox("Copy Assets",
                            "Would you like to copy the engine assets to the current directory?",
                            "yesno", "question", 1) != 0;
      if (copy_assets) {
        if (!CheckFile("engine_assets")) {
          std::filesystem::create_directory("engine_assets");
        }
        for (const auto& entry : std::filesystem::directory_iterator(g_engine_directory_)) {
          std::filesystem::copy(entry.path(), "engine_assets/" + entry.path().filename().string(),
                                std::filesystem::copy_options::overwrite_existing);
        }
      }
    } else {
      debug.Throw(
          "Engine directory selection cancelled, application cannot continue.");
    }
  } else {
    g_engine_directory_ = "engine_assets";
  }
  debug.Log(std::format("Engine directory set to {}", g_engine_directory_));
}

void IIO::IQuit() {
  if (g_level_ != nullptr) {
    g_level_->Quit();
    g_level_.reset();
  }
  for (auto& [key, texture] : g_loaded_textures_) {
    glDeleteTextures(1, &texture->id_);
    texture.reset();
  }
  g_loaded_textures_.clear();
}

///////////////////////////
///  Level IO functions ///
///////////////////////////

std::shared_ptr<Level> IIO::LoadLevel(std::string_view path) {
  auto& debug = engine_->GetDebug();
  if (!CheckFile(path)) {
    debug.Throw(std::format("Requested file does not exist. {}", path));
  }
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.data());
  if (!result) {
    debug.Throw(std::format("Failed to load level file. {}, {}", path, result.description()));
  }

  pugi::xml_node root = doc.child("level");
  if (!root) {
    debug.Throw(std::format("Requested file {} is not a valid level file", path));
  }
  auto level = std::make_shared<Level>(engine_, path);
  engine_->GetAssetManager().AddAsset(level);
  for (pugi::xml_node object_node : root.children("object")) {
    level->objects_.emplace_back(LoadObject(object_node));
  }
  glfwSetWindowTitle(glfwGetCurrentContext(), std::format("Radon Engine - {}", path).c_str());
  level->Init();
  return level;
}

void IIO::SaveLevel(const std::shared_ptr<Level> level, std::string_view path) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("level");
  for (auto& object : level->objects_) {
    auto child = root.append_child("object");
    IIO::SaveObject(object, child);
  }
  bool save_result = doc.save_file(path.data());
  if (!save_result) {
    engine_->GetDebug().Throw(std::format("Failed to save level file. {}", path));
  }
}

////////////////////////////
///  Object IO functions ///
////////////////////////////

std::unique_ptr<Object> IIO::LoadObject(pugi::xml_node& base_node) {
  auto object = std::make_unique<Object>(engine_);
  // Transform is required, so read it first
  auto transform_node = base_node.child("transform");
  if (transform_node != nullptr) {
    object->transform_.Load(transform_node);
  } else {
    engine_->GetDebug().Warning("Object is missing required Transform component, reverting to default values.");
  }

  for (auto& component_node : base_node.children("componentheader")) {
    auto component_type = component_node.attribute("type").as_string();
    if (GetComponentFactory().contains(component_type)) {
      auto component = GetComponentFactory()[component_type]();
      if (component != nullptr) {
        object->AddComponent(component);
      } else {
        engine_->GetDebug().Warning(
            std::format("Failed to create component of type {}, skipping", component_type));
      }
    } else {
      engine_->GetDebug().Warning(
          std::format("Component type {} not found in factory, skipping", component_type));
    }
  }

  object->Load(base_node);
  return object;
}

void IIO::SaveObject(const std::unique_ptr<Object>& object, pugi::xml_node& base_node) {
  // Write transform
  auto transform_node = base_node.append_child("transform");
  object->transform_.Save(transform_node);

  // First, write the component headers
  for (const auto& component : object->GetAllComponents()) {
    pugi::xml_node component_node = base_node.append_child("componentheader");
    component_node.append_attribute("type").set_value(component->GetTypeName());
  }

  object->Save(base_node);
}

///////////////////////////
/// Shader IO functions ///
///////////////////////////

std::shared_ptr<Shader> IIO::LoadShader(std::string_view vertex_path, std::string_view fragment_path) {
  auto& debug = engine_->GetDebug();
  if (!CheckFile(vertex_path) || !CheckFile(fragment_path)) {
    debug.Throw(std::format("Requested shader file does not exist. {}, {}", vertex_path, fragment_path));
  }

  unsigned int vertex = 0;
  unsigned int fragment = 0;
  try {
    vertex = CompileShader(ReadFile(vertex_path), GL_VERTEX_SHADER);
    fragment = CompileShader(ReadFile(fragment_path), GL_FRAGMENT_SHADER);
  } catch (std::runtime_error& e) {
    debug.Throw(std::format("Failed to load shader. {}", e.what()));
    return nullptr;
  }
  unsigned int program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  int success;
  std::array<char, kLogSize> log{};
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == 0) {
    glGetProgramInfoLog(program, kLogSize, nullptr, log.data());
    debug.Throw(std::format("Shader program link failed. {}", log.data()));
    return nullptr;
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  auto shader = std::make_shared<Shader>(engine_, vertex_path, fragment_path);
  engine_->GetAssetManager().AddAsset(shader);
  shader->id_ = program;
  return shader;
}

/////////////////////////////
/// Material IO functions ///
/////////////////////////////

Material* IIO::LoadMaterial(std::string_view diffuse, std::string_view specular,
                            std::string_view vertex, std::string_view fragment, float shininess) {
  std::shared_ptr<Texture> diffuse_texture = nullptr;
  std::shared_ptr<Texture> specular_texture = nullptr;
  std::shared_ptr<Shader> shader = nullptr;
  try {
    diffuse_texture = IIO::LoadTexture(diffuse);
    specular_texture = IIO::LoadTexture(specular);
  } catch (std::runtime_error& e) {
    engine_->GetDebug().Warning(std::format("Material load failed. {}", e.what()));
    diffuse_texture = nullptr;
    specular_texture = nullptr;
  }
  try {
    shader = IIO::LoadShader(vertex, fragment);
    shader ->Use();
    shader ->SetInt("material.diffuse", 0);
    shader ->SetInt("material.specular", 1);
  } catch (std::runtime_error& e) {
    engine_->GetDebug().Warning(std::format("Material load failed. {}", e.what()));
  }
  if (diffuse_texture == nullptr &&
      specular_texture == nullptr &&
      shader == nullptr) {
    engine_->GetDebug().Throw("Material load failed, no valid textures or shader.");
    return nullptr;
  }
  auto path = std::format("{};{};{};{}", diffuse, specular, vertex, fragment);
  auto* material = new Material(engine_, path);
  material->SetPath(std::string(diffuse) + ";" + std::string(specular) + ";" + std::string(vertex) +
                    ";" + std::string(fragment));
  material->shininess_ = shininess;
  return material;
}

Material* IIO::LoadMaterial(pugi::xml_node& node) {
  pugi::xml_node material_node = node.child(kMaterialKeyName);
  if (!material_node) {
    return nullptr;
  }
  return IIO::LoadMaterial(
      material_node.attribute(kMaterialDiffuseKeyName).as_string(),
      material_node.attribute(kMaterialSpecularKeyName).as_string(),
      material_node.attribute(kMaterialVertexKeyName).as_string(),
      material_node.attribute(kMaterialFragmentKeyName).as_string(),
      material_node.attribute(kMaterialShininessKeyName).as_float(kMaterialShininessDefaultValue));
}

void IIO::SaveMaterial(const Material* material, pugi::xml_node& base_node) {
  pugi::xml_node node = base_node.child(kMaterialKeyName);
  if (!node) {
    node = base_node.append_child(kMaterialKeyName);
  }
  if (material->diffuse_ != nullptr) {
    node.append_attribute(kMaterialDiffuseKeyName).set_value(material->diffuse_->GetPath());
  }
  if (material->specular_ != nullptr) {
    node.append_attribute(kMaterialSpecularKeyName).set_value(material->specular_->GetPath());
  }
  if (material->shader_ != nullptr) {
    node.append_attribute(kMaterialVertexKeyName).set_value(material->shader_->vertex_path_);
    node.append_attribute(kMaterialFragmentKeyName).set_value(material->shader_->fragment_path_);
  }
  node.append_attribute(kMaterialShininessKeyName).set_value(material->shininess_);
}

///////////////////////////////
/// Serialized IO functions ///
///////////////////////////////

std::string ValidateName(std::string input);
inline pugi::xml_node GetOrCreateNode(pugi::xml_node& base_node, std::string_view name) {
  auto val_name = ValidateName(name.data());
  pugi::xml_node node = base_node.child(val_name);
  if (!node) {
    node = base_node.append_child(val_name);
  }
  return node;
}

///////////////////////////
/// Helper IO functions ///
///////////////////////////

std::string ValidateName(std::string input) {
  std::ranges::replace(input.begin(), input.end(), ' ', '_');
  return input;
}

std::string IIO::ReadFile(std::string_view path) {
  if (!std::filesystem::exists(path)) {
    engine_->GetDebug().Throw(std::format("Requested file does not exist. {}", path));
  }
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path.data());
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    return stream.str();
  } catch (std::ifstream::failure e) {
    engine_->GetDebug().Throw(std::format("File IO failure. {}, {}", path, e.what()));
  }
  return "";
}

[[nodiscard("Shader Discarded")]] unsigned int IIO::CompileShader(std::string_view code, int type) {
  if (code.empty()) {
    engine_->GetDebug().Throw("Got empty code.");
  }
  const char* code_data = code.data();
  unsigned int shader;
  int success;
  std::array<char, kLogSize> log{};
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &code_data, nullptr);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == 0) {
    glGetShaderInfoLog(shader, kLogSize, nullptr, log.data());
    engine_->GetDebug().Throw(std::format("Shader compilation failed. {}", log.data()));
  }
  return shader;
}
