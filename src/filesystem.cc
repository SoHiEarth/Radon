#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <fmt/core.h>
#include <classes/level.h>
#include <classes/object.h>
#include <classes/shader.h>
#include <classes/texture.h>
#include <engine/filesystem.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture* f::LoadTexture(const std::string_view path) {
  if (!std::filesystem::exists(path)) {
    fmt::print("Requested texture: {} doesn't exist\n", path);
    return nullptr;
  }
  Texture* texture = new Texture(path);
  glGenTextures(1, &texture->id);
  unsigned char* data = stbi_load(path.data(), &texture->w, &texture->h, &texture->channels, 0);
  if (data) {
    GLenum format;
    switch (texture->channels) {
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
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture->w, texture->h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
  } else {
    fmt::print("Failed to load texture {}\n", path);
    stbi_image_free(data);
  }
  return texture;
}

void f::FreeTexture(Texture *texture) {
  if (texture == nullptr) return;
  glDeleteTextures(1, &texture->id);
  delete texture;
}

Level* f::LoadLevel(const std::string_view path) {
  Level* level = new Level();
  return level;
}

void f::LoadLevelDynamicData(const Level* level, const std::string_view path) {

}

void f::SaveLevel(const Level* level, const std::string_view path) {

}

void f::SaveLevelDynamicData(const Level* level, const std::string_view path) {

}

void f::LoadObject(Object* object, const pugi::xml_node& base_node) {

}

void f::SaveObject(const Object* object, const pugi::xml_node& base_node) {

}

const std::string ReadFile(const std::string_view path) {
  if (!std::filesystem::exists(path)) {
    fmt::print("Requested file: {} does not exist.\n", path);
    return "";
  }
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path.data());
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    return stream.str();
  }
  catch (std::ifstream::failure e) {
    fmt::print("Failed to read file.\n");
  }
  return "";
}

unsigned int CompileShader(const std::string_view code, int type) {
  if (code.empty()) return 0;
  const char* code_data = code.data();
  unsigned int shader;
  int success;
  char log[512];
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &code_data, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, log);
    fmt::print("Shader compilation failed.\n{}", log);
    return 0;
  }
  return shader;
}

Shader* f::LoadShader(const std::string_view vert_path, const std::string_view frag_path) {
  Shader* shader = new Shader(vert_path.data(), frag_path.data());
  unsigned int vertex = CompileShader(ReadFile(vert_path), GL_VERTEX_SHADER),
    fragment = CompileShader(ReadFile(frag_path), GL_FRAGMENT_SHADER);
  if (vertex == 0 || fragment == 0) return nullptr;
  unsigned int program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  int success;
  char log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, log);
    fmt::print("Shader program link failed.\n{}", log);
    return nullptr;
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  fmt::print("Successfully loaded shader. Vert: {}, Frag: {}\n", vert_path, frag_path);
  shader->id = program;
  return shader;
}

void f::FreeShader(Shader *shader) {
  if (shader == nullptr) return;
  glDeleteProgram(shader->id);
  delete shader;
}
