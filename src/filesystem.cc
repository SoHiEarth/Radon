#include <classes/level.h>
#include <classes/light.h>
#include <classes/object.h>
#include <classes/shader.h>
#include <classes/sprite.h>
#include <classes/texture.h>
#include <engine/filesystem.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <sstream>
#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define MATERIAL_KEY_NAME "Material"
#define MATERIAL_DIRECTORY_KEY_NAME "directory"
#define MATERIAL_SHININESS_KEY_NAME "shininess"
#define OBJECT_FACTORY_KEY(Object)  \
  {                                 \
    #Object, {                      \
      []() { return new Object(); } \
    }                               \
  }
Level* filesystem::g_level = nullptr;
enum : std::uint16_t { kLogSize = 512 };
std::unordered_map<std::string, std::function<Object*()>> g_object_factory = {
    OBJECT_FACTORY_KEY(Sprite), OBJECT_FACTORY_KEY(DirectionalLight),
    OBJECT_FACTORY_KEY(PointLight), OBJECT_FACTORY_KEY(SpotLight)};

Level* filesystem::LoadLevel(std::string_view path) {
  fmt::print("Loading level {}\n", path);
  auto* level = new Level();
  level->path_ = path;

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.data());
  if (!result) {
    throw std::runtime_error(
        std::format("Failed to parse level file. Details: {}, {}", path, result.description()));
  }

  pugi::xml_node root = doc.child("level");
  if (!root) {
    throw std::runtime_error(
        std::format("Requested file is not a valid level file. Details {}", path));
  }
  for (pugi::xml_node object_node : root.children("object")) {
    fmt::print("Adding object\n");
    level->AddObject(LoadObject(object_node));
  }
  fmt::print("Successfully loaded level {}\n", path);
  return level;
}

void filesystem::SaveLevel(const Level* level, std::string_view path) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("level");

  for (const auto& object : level->objects_) {
    pugi::xml_node object_node = root.append_child("object");
    SaveObject(object, object_node);
  }

  bool save_result = doc.save_file(path.data());
  if (!save_result) {
    throw std::runtime_error(std::format("Failed to save level file. Details: {}", path));
  }
}

Object* filesystem::LoadObject(pugi::xml_node& base_node) {
  std::string type_value = base_node.attribute("type").value();
  auto iterator = g_object_factory.find(type_value);
  if (iterator == g_object_factory.end()) {
    throw std::runtime_error(std::format("Unknown object type: {}", type_value));
  }
  Object* object = iterator->second();
  object->Load(base_node);
  return object;
}

void filesystem::SaveObject(const Object* object, pugi::xml_node& base_node) {
  base_node.append_attribute("type") = object->GetTypeName();
  object->Save(base_node);
}

std::string ReadFile(std::string_view path) {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error(std::format("Requested file does not exist. Details: {}", path));
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
    throw std::runtime_error(std::format("Failed to load file. Details: {}", path));
  }
}

unsigned int CompileShader(std::string_view code, int type) {
  if (code.empty()) {
    throw std::runtime_error(std::format("Got empty code."));
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
    throw std::runtime_error(std::format("Shader compilation failed. Details: {}", log.data()));
  }
  return shader;
}

Shader* filesystem::LoadShader(std::string_view vertex_path, std::string_view fragment_path) {
  auto* shader = new Shader(vertex_path.data(), fragment_path.data());
  unsigned int vertex = CompileShader(ReadFile(vertex_path), GL_VERTEX_SHADER);
  unsigned int fragment = CompileShader(ReadFile(fragment_path), GL_FRAGMENT_SHADER);
  if (vertex == 0 || fragment == 0) {
    throw std::runtime_error(
        std::format("Unknown shader error. Details: {}, {}", vertex_path, fragment_path));
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
    throw std::runtime_error(std::format("Shader program link failed. Details: {}", log.data()));
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  fmt::print("Successfully loaded shader. Vert: {}, Frag: {}\n", vertex_path, fragment_path);
  shader->id_ = program;
  return shader;
}

void filesystem::FreeShader(Shader* shader) {
  if (shader == nullptr) {
    return;
  }
  glDeleteProgram(shader->id_);
  delete shader;
}

Texture* filesystem::LoadTexture(std::string_view path) {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error(std::format("Requested texture doesn't exist. Details: {}", path));
  }
  auto* texture = new Texture();
  glGenTextures(1, &texture->id_);
  stbi_set_flip_vertically_on_load(1);
  unsigned char* data =
      stbi_load(path.data(), &texture->width_, &texture->height_, &texture->channels_, 0);
  if (data == nullptr) {
    throw std::runtime_error(std::format("Failed to load texture. Details: {}", path));
  }
  GLenum format;
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

void filesystem::FreeTexture(Texture* texture) {
  if (texture == nullptr) {
    return;
  }
  glDeleteTextures(1, &texture->id_);
  delete texture;
}

void filesystem::LoadMaterial(Material* material, pugi::xml_node& base_node) {
  pugi::xml_node node = base_node.child(MATERIAL_KEY_NAME);
  material->directory_ = node.attribute(MATERIAL_DIRECTORY_KEY_NAME).as_string();
  material->shininess_ = node.attribute(MATERIAL_SHININESS_KEY_NAME).as_float(1.0F);
}

void filesystem::SaveMaterial(const Material* material, pugi::xml_node& base_node) {
  pugi::xml_node node = base_node.child(MATERIAL_KEY_NAME);
  if (!node) {
    base_node.append_child(MATERIAL_KEY_NAME);
  }
  node.append_attribute(MATERIAL_DIRECTORY_KEY_NAME).set_name(material->directory_);
  node.append_attribute(MATERIAL_SHININESS_KEY_NAME).set_value(material->shininess_);
}
