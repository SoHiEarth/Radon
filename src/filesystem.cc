#include <classes/dynamic_data.h>
#include <classes/level.h>
#include <classes/object.h>
#include <classes/shader.h>
#include <classes/texture.h>
#include <engine/filesystem.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
enum { kLogSize = 512 };

using std::string_view;

Level* f::LoadLevel(const std::string_view kPath) {
  auto* level = new Level();
  level->path_ = kPath;
  // Load objects from path
  return level;
}

void f::LoadLevelDynamicData(Level* level, const std::string_view kPath) {
  auto* dynamic_data = new DynamicData();
  dynamic_data->target_level_ = level->path_;
  // Load dynamic objects from path (XML)
  dynamic_data->Apply(level);
}

void f::SaveLevel(const Level* level, const std::string_view kPath) {}

void f::SaveLevelDynamicData(const Level* level, const std::string_view kPath) {}

void f::LoadObject(Object* object, const pugi::xml_node& base_node) {}

void f::SaveObject(const Object* object, const pugi::xml_node& base_node) {}

std::string ReadFile(const std::string_view kPath) {
  if (!std::filesystem::exists(kPath)) {
    fmt::print("Requested file: {} does not exist.\n", kPath);
    return "";
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
    fmt::print("Failed to read file.\n");
  }
  return "";
}

unsigned int CompileShader(const std::string_view kCode, int type) {
  if (kCode.empty()) {
    return 0;
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
    fmt::print("Shader compilation failed.\n{}", log.data());
    return 0;
  }
  return shader;
}

Shader* f::LoadShader(const string_view kVertPath, const std::string_view kFragPath) {
  auto* shader = new Shader(kVertPath.data(), kFragPath.data());
  unsigned int vertex = CompileShader(ReadFile(kVertPath), GL_VERTEX_SHADER);
  unsigned int fragment = CompileShader(ReadFile(kFragPath), GL_FRAGMENT_SHADER);
  if (vertex == 0 || fragment == 0) {
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
    fmt::print("Shader program link failed.\n{}", log.data());
    return nullptr;
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
    fmt::print("Requested texture: {} doesn't exist\n", kPath);
    return nullptr;
  }
  auto* texture = new Texture(kPath);
  glGenTextures(1, &texture->id_);
  unsigned char* data = stbi_load(kPath.data(), &texture->w_, &texture->h_, &texture->channels_, 0);
  if (data != nullptr) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
  } else {
    fmt::print("Failed to load texture {}\n", kPath);
    stbi_image_free(data);
  }
  return texture;
}

void f::FreeTexture(Texture* texture) {
  if (texture == nullptr) {
    return;
  }
  glDeleteTextures(1, &texture->id_);
  delete texture;
}
