#include <glad/glad.h>
#include <classes/level.h>
#include <classes/object.h>
#include <engine/filesystem.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture f::LoadTexture(const std::string_view path) {
  Texture texture;
  glGenTextures(1, &texture.id);
  glBindTexture(GL_TEXTURE_2D, texture.id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  unsigned char* data = stbi_load(path.data(), &texture.w, &texture.h, &texture.channels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.w, texture.h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  stbi_image_free(data);

  return texture;
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
