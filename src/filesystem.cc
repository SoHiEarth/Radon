#include <classes/dynamic_data.h>
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
enum : std::uint16_t { kLogSize = 512 };
std::unordered_map<std::string, std::function<Object*()>> g_object_factory = {
    {"Sprite", {[]() { return new Sprite(); }}},
    {"DirectionalLight", {[]() { return new DirectionalLight(); }}},
    {"PointLight", {[]() { return new PointLight(); }}},
    {"SpotLight", {[]() { return new SpotLight(); }}}};
using std::string_view;

Level* f::LoadLevel(const std::string_view kPath) {
  fmt::print("Loading level {}\n", kPath);
  auto* level = new Level();
  level->path_ = kPath;

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(kPath.data());
  if (!result) {
    throw std::runtime_error(
        std::format("Failed to parse level file. Details: {}, {}", kPath, result.description()));
  }

  pugi::xml_node root = doc.child("level");
  if (!root) {
    throw std::runtime_error(std::format("Requested file is not a valid level file. Details {}", kPath));
  }
  for (pugi::xml_node object_node : root.children("object")) {
    fmt::print("Adding object\n");
    level->AddObject(LoadObject(object_node));
  }
  fmt::print("Successfully loaded level {}\n", kPath);
  return level;
}

void f::LoadLevelDynamicData(Level* level, const std::string_view kPath) {
  auto* dynamic_data = new DynamicData();
  dynamic_data->target_level_ = level->path_;
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(kPath.data());
  if (!result) {
    throw std::runtime_error(
        std::format("Failed to parse save data. Details: {}, {}", kPath, result.description()));
  }
  pugi::xml_node root = doc.child("data");
  for (pugi::xml_node& object_node : root.children("object")) {
    Object* object = LoadObject(object_node);
    for (auto& old : level->objects_) {
      if (object->name_ == old->name_) {
        delete old;
        old = object;
      }
    }
  }
}

void f::SaveLevel(const Level* level, const std::string_view kPath) {
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("level");

    for (const auto& object : level->objects_) {
      pugi::xml_node object_node = root.append_child("object");
      SaveObject(object, object_node);
    }

    bool save_result = doc.save_file(kPath.data());
    if (!save_result) {
      throw std::runtime_error(
          std::format("Failed to save level file. Details: {}", kPath));
    }
}

void f::SaveLevelDynamicData(const Level* level, const std::string_view kPath) {}

Object* f::LoadObject(pugi::xml_node& base_node) {
    std::string type_value = base_node.attribute("type").value();
    auto it = g_object_factory.find(type_value);
    if (it == g_object_factory.end()) {
        throw std::runtime_error(std::format("Unknown object type: {}", type_value));
    }
    Object* object = it->second();
    object->Load(base_node);
    return object;
}

void f::SaveObject(const Object* object, pugi::xml_node& base_node) {
  base_node.append_attribute("type") = object->GetTypeName();
  object->Save(base_node);
}

std::string ReadFile(const std::string_view kPath) {
  if (!std::filesystem::exists(kPath)) {
    throw std::runtime_error(std::format("Requested file does not exist. Details: {}", kPath));
  }
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(kPath.data());
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    return stream.str();
  } catch (std::ifstream::failure e) {
    throw std::runtime_error(std::format("Failed to load file. Details: {}", kPath));
  }
}

unsigned int CompileShader(const std::string_view kCode, int type) {
  if (kCode.empty()) {
    throw std::runtime_error(std::format("Got empty code."));
  }
  const char* code_data = kCode.data();
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

Shader* f::LoadShader(const string_view kVertPath, const std::string_view kFragPath) {
  auto* shader = new Shader(kVertPath.data(), kFragPath.data());
  unsigned int vertex = CompileShader(ReadFile(kVertPath), GL_VERTEX_SHADER);
  unsigned int fragment = CompileShader(ReadFile(kFragPath), GL_FRAGMENT_SHADER);
  if (vertex == 0 || fragment == 0) {
    throw std::runtime_error(std::format("Unknown shader error. Details: {}, {}", kVertPath, kFragPath));
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
  fmt::print("Successfully loaded shader. Vert: {}, Frag: {}\n", kVertPath, kFragPath);
  shader->id_ = program;
  return shader;
}

void f::FreeShader(Shader* shader) {
  if (shader == nullptr) {
    return;
  }
  glDeleteProgram(shader->id_);
  delete shader;
}

Texture* f::LoadTexture(const std::string_view kPath) {
  if (!std::filesystem::exists(kPath)) {
    throw std::runtime_error(std::format("Requested texture doesn't exist. Details: {}", kPath));
  }
  auto* texture = new Texture(kPath);
  glGenTextures(1, &texture->id_);
  stbi_set_flip_vertically_on_load(1);
  unsigned char* data = stbi_load(kPath.data(), &texture->w_, &texture->h_, &texture->channels_, 0);
  if (data == nullptr) {
    throw std::runtime_error(std::format("Failed to load texture. Details: {}", kPath));
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
  glTexImage2D(GL_TEXTURE_2D, 0, format, texture->w_, texture->h_, 0, format, GL_UNSIGNED_BYTE,
               data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  stbi_image_free(data);
  return texture;
}

void f::FreeTexture(Texture* texture) {
  if (texture == nullptr) {
    return;
  }
  glDeleteTextures(1, &texture->id_);
  delete texture;
}
