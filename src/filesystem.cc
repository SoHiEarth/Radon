#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <fmt/core.h>
#include <classes/level.h>
#include <classes/object.h>
#include <classes/shader.h>
#include <engine/filesystem.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <filesystem>

Texture* f::LoadTexture(const std::string_view path) {
  if (!std::filesystem::exists(path)) {
    fmt::print("Requested textures don't exist\n");
    return nullptr;
  }
  Texture* texture = new Texture;
  glGenTextures(1, &texture->id);
  glBindTexture(GL_TEXTURE_2D, texture->id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(path.data(), &texture->w, &texture->h, &texture->channels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->w, texture->h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    fmt::print("Failed to load texture {}\n", path);
  }
  stbi_image_free(data);
  return texture;
}

void f::FreeTexture(Texture *texture) {
  if (!texture) return;
  glDeleteTextures(1, &texture->id);
  delete texture;
}

Level f::LoadLevel(const std::string_view path) {
  Level level;

  return level;
}

void f::LoadLevelDynamicData(const Level* level, const std::string_view path) {

}

// Saves the data of the entire level, including static and dynamic objects
void f::SaveLevel(const Level* level, const std::string_view path) {

}

// Saves the data for dynamic objects in the level, for use as save files
void f::SaveLevelDynamicData(const Level* level, const std::string_view path) {

}

void f::LoadObject(Object* object, const pugi::xml_node& base_node) {

}

void f::SaveObject(const Object* object, const pugi::xml_node& base_node) {

}

const std::string ReadFile(const std::string_view path) {
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
    fmt::print("Shader Compilation failed. \nGL Info:{}\n", log);
  }
  return shader;
}

Shader* f::LoadShader(const std::string_view path) {
  Shader* shader = new Shader();
  unsigned int vertex = CompileShader(ReadFile(std::string(path) + "/vert.glsl"),
      GL_VERTEX_SHADER),
               fragment = CompileShader(ReadFile(std::string(path) + "/frag.glsl"),
      GL_FRAGMENT_SHADER);
  unsigned int program;
  program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  int success;
  char log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, log);
    fmt::print("Shader program link failed. \nGL Info:{}\n", log);
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  fmt::print("Successfully loaded shader\n");
  shader->id = program;
  return shader;
}

void f::FreeShader(Shader *shader) {
  glDeleteProgram(shader->id);
  delete shader;
}
