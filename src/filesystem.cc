#include <classes/level.h>
#include <classes/light.h>
#include <classes/object.h>
#include <classes/shader.h>
#include <classes/sprite.h>
#include <classes/texture.h>
#include <engine/debug.h>
#include <engine/filesystem.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <algorithm>
#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <sstream>
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
#define OBJECT_FACTORY_KEY(Object)  \
  {                                 \
    #Object, {                      \
      []() { return new Object(); } \
    }                               \
  }
Level* filesystem::g_level = nullptr;
enum : std::uint16_t { kLogSize = 512 };
std::unordered_map<std::string, std::function<Object*()>> filesystem::g_object_factory = {
    OBJECT_FACTORY_KEY(Sprite), OBJECT_FACTORY_KEY(DirectionalLight),
    OBJECT_FACTORY_KEY(PointLight), OBJECT_FACTORY_KEY(SpotLight)};
std::string ValidateName(std::string input);

///////////////////////////
///  Level IO functions ///
///////////////////////////

Level* filesystem::serialized::LoadLevel(std::string_view path) {
  if (path.empty()) {
    debug::Throw(GET_TRACE, "Requested file path is empty.");
  }
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.data());
  if (!result) {
    debug::Throw(GET_TRACE,
                 std::format("Failed to load level file. {}, {}", path, result.description()));
  }

  pugi::xml_node root = doc.child("level");
  if (!root) {
    debug::Throw(GET_TRACE, std::format("Requested file {} is not a valid level file", path));
  }

  auto* level = new Level();
  level->path_ = path;
  for (pugi::xml_node object_node : root.children("object")) {
    level->AddObject(filesystem::serialized::LoadObject(object_node));
  }
  debug::Log(GET_TRACE, std::format("Successfully loaded {}", path));
  level->Init();
  return level;
}

void filesystem::FreeLevel(Level* level) {
  if (level == nullptr) {
    return;
  }
  for (auto* object : level->objects_) {
    object->Quit();
    object->has_quit_ = true;
    FreeObject(object);
  }
  delete level;
  level = nullptr;
}

void filesystem::serialized::SaveLevel(const Level* level, std::string_view path) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("level");
  for (const auto& object : level->objects_) {
    pugi::xml_node object_node = root.append_child("object");
    object_node.append_attribute("type") = object->GetTypeName();
    object->Save(object_node);
  }
  bool save_result = doc.save_file(path.data());
  if (!save_result) {
    debug::Throw(GET_TRACE, std::format("Failed to save level file. {}", path));
  }
}

////////////////////////////
///  Object IO functions ///
////////////////////////////

Object* filesystem::serialized::LoadObject(pugi::xml_node& base_node) {
  std::string type_value = base_node.attribute("type").value();
  auto iterator = g_object_factory.find(type_value);
  if (iterator == g_object_factory.end()) {
    debug::Throw(GET_TRACE, std::format("Unknown object type: {}", type_value));
  }
  Object* object = iterator->second();
  object->Load(base_node);
  return object;
}

void filesystem::FreeObject(Object*& object) {
  if (object == nullptr) {
    return;
  }
  if (object->has_initialized_ && !object->has_quit_) {
    object->Quit();
    object->has_quit_ = true;
  }
  delete object;
  object = nullptr;
}

void filesystem::serialized::SaveObject(const Object* object, pugi::xml_node& base_node) {
  if (object == nullptr) {
    return;
  }
  object->Save(base_node);
}

///////////////////////////
/// Shader IO functions ///
///////////////////////////

std::string ReadFile(std::string_view path);
unsigned int CompileShader(std::string_view code, int type);

Shader* filesystem::LoadShader(std::string_view vertex_path, std::string_view fragment_path) {
  unsigned int vertex = 0;
  unsigned int fragment = 0;
  try {
    vertex = CompileShader(ReadFile(vertex_path), GL_VERTEX_SHADER);
    fragment = CompileShader(ReadFile(fragment_path), GL_FRAGMENT_SHADER);
  } catch (std::runtime_error& e) {
    debug::Throw(GET_TRACE, std::format("Failed to load shader. {}", e.what()));
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
    debug::Throw(GET_TRACE, std::format("Shader program link failed. {}", log.data()));
    return nullptr;
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  auto* shader = new Shader(vertex_path, fragment_path);
  shader->id_ = program;
  return shader;
}

void filesystem::FreeShader(Shader*& shader) {
  if (shader == nullptr) {
    return;
  }
  glDeleteProgram(shader->id_);
  delete shader;
  shader = nullptr;
}

////////////////////////////
/// Texture IO functions ///
////////////////////////////

Texture* filesystem::LoadTexture(std::string_view path) {
  auto* texture = new Texture();
  texture->path_ = path;
  glGenTextures(1, &texture->id_);
  stbi_set_flip_vertically_on_load(1);
  unsigned char* data =
      stbi_load(path.data(), &texture->width_, &texture->height_, &texture->channels_, 0);
  if (data == nullptr) {
    delete texture;
    texture = nullptr;
    debug::Throw(GET_TRACE, std::format("Failed to load texture. Details: {}", path));
    return nullptr;
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

void filesystem::FreeTexture(Texture*& texture) {
  if (texture == nullptr) {
    return;
  }
  glDeleteTextures(1, &texture->id_);
  delete texture;
  texture = nullptr;
}

/////////////////////////////
/// Material IO functions ///
/////////////////////////////

Material* filesystem::LoadMaterial(std::string_view diffuse, std::string_view specular,
                                   std::string_view vertex, std::string_view fragment,
                                   float shininess) {
  auto* material = new Material();
  material->shininess_ = shininess;
  try {
    material->diffuse_ = filesystem::LoadTexture(diffuse);
    material->specular_ = filesystem::LoadTexture(specular);
    material->shader_ = filesystem::LoadShader(vertex, fragment);
  } catch (std::runtime_error& e) {
    debug::Log(GET_TRACE, std::format("Material load failed. {}", e.what()));
    filesystem::FreeMaterial(material);
    return nullptr;
  }
  material->shader_->Use();
  material->shader_->SetInt("material.diffuse", 0);
  material->shader_->SetInt("material.specular", 1);
  material->is_initialized_ = true;
  return material;
}

void filesystem::FreeMaterial(Material*& material) {
  if (material == nullptr) {
    return;
  }
  filesystem::FreeTexture(material->diffuse_);
  filesystem::FreeTexture(material->specular_);
  filesystem::FreeShader(material->shader_);
  delete material;
  material = nullptr;
}

Material* filesystem::serialized::LoadMaterial(pugi::xml_node& node) {
  pugi::xml_node material_node = node.child(MATERIAL_KEY_NAME);
  if (!material_node) {
    return nullptr;
  }
  return filesystem::LoadMaterial(material_node.attribute(MATERIAL_DIFFUSE_KEY_NAME).as_string(),
                                  material_node.attribute(MATERIAL_SPECULAR_KEY_NAME).as_string(),
                                  material_node.attribute(MATERIAL_VERTEX_KEY_NAME).as_string(),
                                  material_node.attribute(MATERIAL_FRAGMENT_KEY_NAME).as_string(),
                                  material_node.attribute(MATERIAL_SHININESS_KEY_NAME)
                                      .as_float(MATERIAL_SHININESS_DEFAULT_VALUE));
}

void filesystem::serialized::SaveMaterial(const Material* material, pugi::xml_node& base_node) {
  if (material == nullptr) {
    return;
  }

  if (!material->IsValid()) {
    debug::Log(GET_TRACE, "Material is invalid (missing texture or shader), skipping save");
    return;
  }

  pugi::xml_node node = base_node.child(MATERIAL_KEY_NAME);
  if (!node) {
    node = base_node.append_child(MATERIAL_KEY_NAME);
  }
  node.append_attribute(MATERIAL_DIFFUSE_KEY_NAME).set_value(material->diffuse_->path_);
  node.append_attribute(MATERIAL_SPECULAR_KEY_NAME).set_value(material->specular_->path_);
  node.append_attribute(MATERIAL_VERTEX_KEY_NAME).set_value(material->shader_->vertex_path_);
  node.append_attribute(MATERIAL_FRAGMENT_KEY_NAME).set_value(material->shader_->fragment_path_);
  node.append_attribute(MATERIAL_SHININESS_KEY_NAME).set_value(material->shininess_);
}

///////////////////////////////
/// Serialized IO functions ///
///////////////////////////////

glm::vec3 filesystem::serialized::LoadVec3(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  glm::vec3 value;
  value.x = node.attribute("x").as_float(0.0F);
  value.y = node.attribute("y").as_float(0.0F);
  value.z = node.attribute("z").as_float(0.0F);
  return value;
}

glm::vec2 filesystem::serialized::LoadVec2(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  glm::vec2 value;
  value.x = node.attribute("x").as_float(0.0F);
  value.y = node.attribute("y").as_float(0.0F);
  return value;
}

std::string filesystem::serialized::LoadString(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  return node.attribute("value").as_string("");
}

int filesystem::serialized::LoadInt(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  return node.attribute("value").as_int(0);
}

float filesystem::serialized::LoadFloat(pugi::xml_node& base_node, std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  return node.attribute("value").as_float(0.0F);
}

void filesystem::serialized::SaveVec3(const glm::vec3* value, pugi::xml_node& base_node,
                                      std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
  node.append_attribute("x").set_value(value->x);
  node.append_attribute("y").set_value(value->y);
  node.append_attribute("z").set_value(value->z);
}

void filesystem::serialized::SaveVec2(const glm::vec2* value, pugi::xml_node& base_node,
                                      std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
  node.append_attribute("x").set_value(value->x);
  node.append_attribute("y").set_value(value->y);
}

void filesystem::serialized::SaveString(const std::string* value, pugi::xml_node& base_node,
                                        std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
  node.append_attribute("value").set_value(value->c_str());
}

void filesystem::serialized::SaveInt(const int* value, pugi::xml_node& base_node,
                                     std::string name) {
  name = ValidateName(name);
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
  node.append_attribute("value").set_value(*value);
}

void filesystem::serialized::SaveFloat(const float* value, pugi::xml_node& base_node,
                                       std::string name) {
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
    debug::Throw(GET_TRACE, std::format("Requested file does not exist. {}", path));
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
    debug::Throw(GET_TRACE, std::format("File IO failure. {}, {}", path, e.what()));
  }
  return "";
}

unsigned int CompileShader(std::string_view code, int type) {
  if (code.empty()) {
    debug::Throw(GET_TRACE, "Got empty code.");
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
    debug::Throw(GET_TRACE, std::format("Shader compilation failed. {}", log.data()));
  }
  return shader;
}
