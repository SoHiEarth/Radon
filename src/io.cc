#include <classes/level.h>
#include <classes/light.h>
#include <classes/object.h>
#include <classes/shader.h>
#include <classes/sprite.h>
#include <classes/texture.h>
#include <engine/debug.h>
#include <engine/io.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <algorithm>
#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <sstream>
#include <GLFW/glfw3.h>
#include <pugixml.hpp>
#include <format>
#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define MATERIAL_KEY_NAME "Material"
#define MATERIAL_DIFFUSE_KEY_NAME "diffuse"
#define MATERIAL_SPECULAR_KEY_NAME "specular"
#define MATERIAL_VERTEX_KEY_NAME "vs"
#define MATERIAL_FRAGMENT_KEY_NAME "fs"
#define MATERIAL_SHININESS_KEY_NAME "shininess"
#define MATERIAL_SHININESS_DEFAULT_VALUE 32.0F
#define OBJECT_FACTORY_KEY(Object)                \
  {                                               \
    #Object, {                                    \
      []() { return std::make_unique<Object>(); } \
    }                                             \
  }

enum : std::uint16_t { kLogSize = 512 };
std::unique_ptr<Level> io::g_level;
std::string io::g_engine_directory;
std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>> io::g_component_factory = {
    OBJECT_FACTORY_KEY(Sprite), OBJECT_FACTORY_KEY(DirectionalLight),
    OBJECT_FACTORY_KEY(PointLight), OBJECT_FACTORY_KEY(SpotLight)};
std::string ValidateName(std::string input);
bool CheckFile(std::string_view path) {
  return std::filesystem::exists(path);
}

void io::Init() {
  if (!CheckFile("engine_assets")) {
    tinyfd_messageBox("Engine Directory Not Found",
                      "Please select the engine directory to continue.", "ok", "info", 1);
    char* engine_path = tinyfd_selectFolderDialog("Select the engine directory",
                                                  std::filesystem::current_path().string().c_str());
    if (engine_path != nullptr) {
      g_engine_directory = std::string(engine_path);
      bool copy_assets = tinyfd_messageBox(
          "Copy Assets", "Would you like to copy the engine assets to the current directory?",
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
      debug::Throw("Engine directory selection cancelled, application cannot continue.");
    }
  } else {
    io::g_engine_directory = "engine_assets";
  }
  debug::Log("Initialized io");
}

///////////////////////////
///  Level IO functions ///
///////////////////////////

std::unique_ptr<Level> io::xml::LoadLevel(std::string_view path) {
  if (!CheckFile(path)) {
    debug::Throw(std::format("Requested file does not exist. {}", path));
  }
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.data());
  if (!result) {
    debug::Throw(std::format("Failed to load level file. {}, {}", path, result.description()));
  }

  pugi::xml_node root = doc.child("level");
  if (!root) {
    debug::Throw(std::format("Requested file {} is not a valid level file", path));
  }

  auto level = std::make_unique<Level>();
  level->path_ = path;
  for (pugi::xml_node object_node : root.children("object")) {
    level->AddObject(io::xml::LoadObject(object_node));
  }
  glfwSetWindowTitle(glfwGetCurrentContext(), std::format("Radon Engine - {}", path).c_str());
  debug::Log(std::format("Successfully loaded {}", path));
  level->Init();
  return level;
}

void io::xml::SaveLevel(const Level& level, std::string_view path) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("level");
  for (const auto& object : level.objects_) {
    pugi::xml_node object_node = root.append_child("object");
    object->Save(object_node);
  }
  bool save_result = doc.save_file(path.data());
  if (!save_result) {
    debug::Throw(std::format("Failed to save level file. {}", path));
  }
}

////////////////////////////
///  Object IO functions ///
////////////////////////////

std::shared_ptr<Object> io::xml::LoadObject(pugi::xml_node& base_node) {
  auto object = std::make_shared<Object>();
  object->Load(base_node);
  return object;
}

void io::xml::SaveObject(const Object& object, pugi::xml_node& base_node) {
  object.Save(base_node);
}

///////////////////////////
/// Shader IO functions ///
///////////////////////////

std::string ReadFile(std::string_view path);
unsigned int CompileShader(std::string_view code, int type);

std::unique_ptr<Shader> io::LoadShader(std::string_view vertex_path,
                                       std::string_view fragment_path) {
  if (!CheckFile(vertex_path) || !CheckFile(fragment_path)) {
    debug::Throw(
        std::format("Requested shader file does not exist. {}, {}", vertex_path, fragment_path));
  }

  unsigned int vertex = 0;
  unsigned int fragment = 0;
  try {
    vertex = CompileShader(ReadFile(vertex_path), GL_VERTEX_SHADER);
    fragment = CompileShader(ReadFile(fragment_path), GL_FRAGMENT_SHADER);
  } catch (std::runtime_error& e) {
    debug::Throw(std::format("Failed to load shader. {}", e.what()));
    return nullptr;
  }
  unsigned int program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  int success;
  std::array<char, kLogSize> log;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == 0) {
    glGetProgramInfoLog(program, kLogSize, nullptr, log.data());
    debug::Throw(std::format("Shader program link failed. {}", log.data()));
    return nullptr;
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  auto shader = std::make_unique<Shader>(vertex_path, fragment_path);
  shader->id_ = program;
  return shader;
}

////////////////////////////
/// Texture IO functions ///
////////////////////////////

std::unique_ptr<Texture> io::LoadTexture(std::string_view path) {
  auto texture = std::make_unique<Texture>();
  texture->path_ = path;
  glGenTextures(1, &texture->id_);
  stbi_set_flip_vertically_on_load(1);
  unsigned char* data =
      stbi_load(path.data(), &texture->width_, &texture->height_, &texture->channels_, 0);
  if (data == nullptr) {
    debug::Throw(std::format("Failed to load texture. Details: {}", path));
  }
  GLenum format = GL_RGB;
  switch (texture->channels_) {
    case 1:
      format = GL_RED;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
  }
  glBindTexture(GL_TEXTURE_2D, texture->id_);
  glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width_, texture->height_, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  stbi_image_free(data);
  return texture;
}

/////////////////////////////
/// Material IO functions ///
/////////////////////////////

std::shared_ptr<Material> io::LoadMaterial(std::string_view diffuse,
                           std::string_view specular,
                           std::string_view vertex,
                           std::string_view fragment,
                           float shininess) {
  auto material = std::make_shared<Material>();
  material->shininess_ = shininess;
  try {
    material->diffuse_ = io::LoadTexture(diffuse);
    material->specular_ = io::LoadTexture(specular);
    material->shader_ = io::LoadShader(vertex, fragment);
  } catch (std::runtime_error& e) {
    debug::Warning(std::format("Material load failed. {}", e.what()));
  }
  material->shader_->Use();
  material->shader_->SetInt("material.diffuse", 0);
  material->shader_->SetInt("material.specular", 1);
  material->is_initialized_ = true;
  return material;
}

std::shared_ptr<Material> io::xml::LoadMaterial(pugi::xml_node& node) {
  pugi::xml_node material_node = node.child(MATERIAL_KEY_NAME);
  if (!material_node) {
    return nullptr;
  }
  return io::LoadMaterial(material_node.attribute(MATERIAL_DIFFUSE_KEY_NAME).as_string(),
                          material_node.attribute(MATERIAL_SPECULAR_KEY_NAME).as_string(),
                          material_node.attribute(MATERIAL_VERTEX_KEY_NAME).as_string(),
                          material_node.attribute(MATERIAL_FRAGMENT_KEY_NAME).as_string(),
                          material_node.attribute(MATERIAL_SHININESS_KEY_NAME)
                              .as_float(MATERIAL_SHININESS_DEFAULT_VALUE));
}

void io::xml::SaveMaterial(const Material& material, pugi::xml_node& base_node) {

  if (!material.IsValid()) {
    debug::Log("Material is invalid (missing texture or shader), skipping save");
    return;
  }

  pugi::xml_node node = base_node.child(MATERIAL_KEY_NAME);
  if (!node) {
    node = base_node.append_child(MATERIAL_KEY_NAME);
  }
  node.append_attribute(MATERIAL_DIFFUSE_KEY_NAME).set_value(material.diffuse_->path_);
  node.append_attribute(MATERIAL_SPECULAR_KEY_NAME).set_value(material.specular_->path_);
  node.append_attribute(MATERIAL_VERTEX_KEY_NAME).set_value(material.shader_->vertex_path_);
  node.append_attribute(MATERIAL_FRAGMENT_KEY_NAME).set_value(material.shader_->fragment_path_);
  node.append_attribute(MATERIAL_SHININESS_KEY_NAME).set_value(material.shininess_);
}

///////////////////////////////
/// Serialized IO functions ///
///////////////////////////////

glm::vec3 io::xml::LoadVec3(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  glm::vec3 value;
  value.x = node.attribute("x").as_float(0.0F);
  value.y = node.attribute("y").as_float(0.0F);
  value.z = node.attribute("z").as_float(0.0F);
  return value;
}

glm::vec2 io::xml::LoadVec2(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  glm::vec2 value;
  value.x = node.attribute("x").as_float(0.0F);
  value.y = node.attribute("y").as_float(0.0F);
  return value;
}

std::string io::xml::LoadString(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  return node.attribute("value").as_string("");
}

int io::xml::LoadInt(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  return node.attribute("value").as_int(0);
}

float io::xml::LoadFloat(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  return node.attribute("value").as_float(0.0F);
}

void io::xml::SaveVec3(const glm::vec3& value, pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
  node.append_attribute("x").set_value(value.x);
  node.append_attribute("y").set_value(value.y);
  node.append_attribute("z").set_value(value.z);
}

void io::xml::SaveVec2(const glm::vec2& value, pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
  node.append_attribute("x").set_value(value.x);
  node.append_attribute("y").set_value(value.y);
}

void io::xml::SaveString(std::string_view value, pugi::xml_node& base_node,
                                std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
  node.append_attribute("value").set_value(value.data());
}

void io::xml::SaveInt(const int* value, pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
  node.append_attribute("value").set_value(*value);
}

void io::xml::SaveFloat(const float* value, pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
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
    debug::Throw(std::format("Requested file does not exist. {}", path));
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
    debug::Throw(std::format("File IO failure. {}, {}", path, e.what()));
  }
  return "";
}

[[nodiscard("Shader Discarded")]] unsigned int CompileShader(std::string_view code, int type) {
  if (code.empty()) {
    debug::Throw("Got empty code.");
  }
  const char* code_data = code.data();
  unsigned int shader;
  int success;
  std::array<char, kLogSize> log;
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &code_data, nullptr);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == 0) {
    glGetShaderInfoLog(shader, kLogSize, nullptr, log.data());
    debug::Throw(std::format("Shader compilation failed. {}", log.data()));
  }
  return shader;
}
