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
#define OBJECT_FACTORY_KEY(Object)  \
  {                                 \
    #Object, {                      \
      []() { return new Object(); } \
    }                               \
  }

enum : std::uint16_t { kLogSize = 512 };
std::unordered_map<std::string_view, std::function<Component*()>> IIO::g_component_factory = {
    OBJECT_FACTORY_KEY(Transform),        OBJECT_FACTORY_KEY(ModelRenderer),
    OBJECT_FACTORY_KEY(AudioSource),      OBJECT_FACTORY_KEY(PhysicsObject),
    OBJECT_FACTORY_KEY(DirectionalLight), OBJECT_FACTORY_KEY(PointLight),
    OBJECT_FACTORY_KEY(SpotLight)};
std::string ValidateName(std::string input);
bool IIO::CheckFile(std::string_view path) {
  return std::filesystem::exists(path);
}

void IIO::i_Init() {
  if (!CheckFile("engine_assets")) {
    tinyfd_messageBox("Engine Directory Not Found",
                      "Please select the engine directory to continue.", "ok", "info", 1);
    char* engine_path = tinyfd_selectFolderDialog("Select the engine directory",
                                                  std::filesystem::current_path().string().c_str());
    if (engine_path != nullptr) {
      g_engine_directory = std::string(engine_path);
      bool copy_assets =
          tinyfd_messageBox("Copy Assets",
                            "Would you like to copy the engine assets to the current directory?",
                            "yesno", "question", 1) != 0;
      if (copy_assets) {
        if (!CheckFile("engine_assets")) {
          std::filesystem::create_directory("engine_assets");
        }
        for (const auto& entry : std::filesystem::directory_iterator(g_engine_directory)) {
          std::filesystem::copy(entry.path(), "engine_assets/" + entry.path().filename().string(),
                                std::filesystem::copy_options::overwrite_existing);
        }
      }
    } else {
      IDebug::Throw("Engine directory selection cancelled, application cannot continue.");
    }
  } else {
    IIO::g_engine_directory = "engine_assets";
  }
}

void IIO::i_Quit() {
  if (g_level != nullptr) {
    g_level->Quit();
    delete g_level;
    g_level = nullptr;
  }
  for (auto& [key, texture] : g_loaded_textures) {
    glDeleteTextures(1, &texture->id_);
    delete texture;
  }
  g_loaded_textures.clear();
}

///////////////////////////
///  Level IO functions ///
///////////////////////////

Level* IIO::LoadLevel(std::string_view path) {
  if (!CheckFile(path)) {
    IDebug::Throw(std::format("Requested file does not exist. {}", path));
  }
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.data());
  if (!result) {
    IDebug::Throw(std::format("Failed to load level file. {}, {}", path, result.description()));
  }

  pugi::xml_node root = doc.child("level");
  if (!root) {
    IDebug::Throw(std::format("Requested file {} is not a valid level file", path));
  }

  auto level = new Level();
  level->path_ = strcpy(new char[path.size() + 1], path.data());
  for (pugi::xml_node object_node : root.children("object")) {
    level->AddObject(IIO::LoadObject(object_node));
  }
  glfwSetWindowTitle(glfwGetCurrentContext(), std::format("Radon Engine - {}", path).c_str());
  level->Init();
  return level;
}

void IIO::SaveLevel(const Level* level, std::string_view path) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("level");
  for (Object* object : level->objects_) {
    auto child = root.append_child("object");
    IIO::Get<IIO>().SaveObject(object, child);
  }
  bool save_result = doc.save_file(path.data());
  if (!save_result) {
    IDebug::Throw(std::format("Failed to save level file. {}", path));
  }
}

////////////////////////////
///  Object IO functions ///
////////////////////////////

Object* IIO::LoadObject(pugi::xml_node& base_node) {
  auto object = new Object();

  // Transform is required, so read it first
  auto transform_node = base_node.child("transform");
  if (transform_node != nullptr) {
    object->transform_.Load(transform_node);
  } else {
    IDebug::Warning("Object is missing required Transform component, reverting to default values.");
  }

  for (auto& component_node : base_node.children("componentheader")) {
    std::string component_type = component_node.attribute("type").as_string();
    if (g_component_factory.contains(component_type)) {
      Component* component = g_component_factory[component_type]();
      if (component != nullptr) {
        object->AddComponent(component);
      } else {
        IDebug::Warning(
            std::format("Failed to create component of type {}, skipping", component_type));
      }
    } else {
      IDebug::Warning(
          std::format("Component type {} not found in factory, skipping", component_type));
    }
  }

  object->Load(base_node);
  return object;
}

void IIO::SaveObject(Object*& object, pugi::xml_node& base_node) {
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

std::string ReadFile(std::string_view path);
unsigned int CompileShader(std::string_view code, int type);

Shader* IIO::LoadShader(std::string_view vertex_path, std::string_view fragment_path) {
  if (!CheckFile(vertex_path) || !CheckFile(fragment_path)) {
    IDebug::Throw(
        std::format("Requested shader file does not exist. {}, {}", vertex_path, fragment_path));
  }

  unsigned int vertex = 0;
  unsigned int fragment = 0;
  try {
    vertex = CompileShader(ReadFile(vertex_path), GL_VERTEX_SHADER);
    fragment = CompileShader(ReadFile(fragment_path), GL_FRAGMENT_SHADER);
  } catch (std::runtime_error& e) {
    IDebug::Throw(std::format("Failed to load shader. {}", e.what()));
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
    IDebug::Throw(std::format("Shader program link failed. {}", log.data()));
    return nullptr;
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  auto shader = new Shader(vertex_path.data(), fragment_path.data());
  shader->id_ = program;
  return shader;
}

/////////////////////////////
/// Material IO functions ///
/////////////////////////////

Material* IIO::LoadMaterial(std::string_view diffuse, std::string_view specular,
                            std::string_view vertex, std::string_view fragment, float shininess) {
  auto material = new Material();
  material->shininess_ = shininess;
  try {
    material->diffuse_ = IIO::LoadTexture(diffuse);
    material->specular_ = IIO::LoadTexture(specular);
  } catch (std::runtime_error& e) {
    IDebug::Warning(std::format("Material load failed. {}", e.what()));
  }
  try {
    material->shader_ = IIO::LoadShader(vertex, fragment);
    material->shader_->Use();
    material->shader_->SetInt("material.diffuse", 0);
    material->shader_->SetInt("material.specular", 1);
  } catch (std::runtime_error& e) {
    IDebug::Warning(std::format("Material load failed. {}", e.what()));
  }
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
  if (material->diffuse_)
    node.append_attribute(kMaterialDiffuseKeyName).set_value(material->diffuse_->path_);
  if (material->specular_)
    node.append_attribute(kMaterialSpecularKeyName).set_value(material->specular_->path_);
  if (material->shader_) {
    node.append_attribute(kMaterialVertexKeyName).set_value(material->shader_->vertex_path_);
    node.append_attribute(kMaterialFragmentKeyName).set_value(material->shader_->fragment_path_);
  }
  node.append_attribute(kMaterialShininessKeyName).set_value(material->shininess_);
}

///////////////////////////////
/// Serialized IO functions ///
///////////////////////////////

glm::vec3 IIO::LoadVec3(pugi::xml_node& base_node, std::string_view name) {
  pugi::xml_node node = base_node.child(ValidateName(name.data()));
  glm::vec3 value{};
  value.x = node.attribute("x").as_float(0.0F);
  value.y = node.attribute("y").as_float(0.0F);
  value.z = node.attribute("z").as_float(0.0F);
  return value;
}

glm::vec2 IIO::LoadVec2(pugi::xml_node& base_node, std::string_view name) {
  pugi::xml_node node = base_node.child(ValidateName(name.data()));
  glm::vec2 value{};
  value.x = node.attribute("x").as_float(0.0F);
  value.y = node.attribute("y").as_float(0.0F);
  return value;
}

std::string IIO::LoadString(pugi::xml_node& base_node, std::string_view name) {
  pugi::xml_node node = base_node.child(ValidateName(name.data()));
  return node.attribute("value").as_string("");
}

int IIO::LoadInt(pugi::xml_node& base_node, std::string_view name) {
  pugi::xml_node node = base_node.child(ValidateName(name.data()));
  return node.attribute("value").as_int(0);
}

float IIO::LoadFloat(pugi::xml_node& base_node, std::string_view name) {
  pugi::xml_node node = base_node.child(ValidateName(name.data()));
  return node.attribute("value").as_float(0.0F);
}

inline pugi::xml_node GetOrCreateNode(pugi::xml_node& base_node, std::string_view name) {
  auto val_name = ValidateName(name.data());
  pugi::xml_node node = base_node.child(val_name);
  if (!node) {
    node = base_node.append_child(val_name);
  }
  return node;
}

void IIO::SaveVec3(const glm::vec3& value, pugi::xml_node& base_node, std::string_view name) {
  auto node = GetOrCreateNode(base_node, name);
  node.append_attribute("x").set_value(value.x);
  node.append_attribute("y").set_value(value.y);
  node.append_attribute("z").set_value(value.z);
}

void IIO::SaveVec2(const glm::vec2& value, pugi::xml_node& base_node, std::string_view name) {
  auto node = GetOrCreateNode(base_node, name);
  node.append_attribute("x").set_value(value.x);
  node.append_attribute("y").set_value(value.y);
}

void IIO::SaveString(std::string_view value, pugi::xml_node& base_node, std::string_view name) {
  auto node = GetOrCreateNode(base_node, name);
  node.append_attribute("value").set_value(value.data());
}

void IIO::SaveInt(const int* value, pugi::xml_node& base_node, std::string_view name) {
  auto node = GetOrCreateNode(base_node, name);
  node.append_attribute("value").set_value(*value);
}

void IIO::SaveFloat(const float* value, pugi::xml_node& base_node, std::string_view name) {
  auto node = GetOrCreateNode(base_node, name);
  node.append_attribute("value").set_value(*value);
}

///////////////////////////
/// Helper IO functions ///
///////////////////////////

std::string ValidateName(std::string input) {
  std::ranges::replace(input.begin(), input.end(), ' ', '_');
  return input;
}

std::string ReadFile(std::string_view path) {
  if (!std::filesystem::exists(path)) {
    IDebug::Throw(std::format("Requested file does not exist. {}", path));
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
    IDebug::Throw(std::format("File IO failure. {}, {}", path, e.what()));
  }
  return "";
}

[[nodiscard("Shader Discarded")]] unsigned int CompileShader(std::string_view code, int type) {
  if (code.empty()) {
    IDebug::Throw("Got empty code.");
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
    IDebug::Throw(std::format("Shader compilation failed. {}", log.data()));
  }
  return shader;
}
