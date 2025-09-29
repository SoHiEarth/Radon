#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <engine/io.h>
#include <engine/debug.h>
#include <classes/texture.h>
#include <format>
#include <map>
#include <string>

////////////////////////////
/// Texture IO functions ///
////////////////////////////

Texture* IIO::LoadTexture(std::string_view path) {
  for (const auto& [key, value] : g_loaded_textures) {
    if (key == path) {
      return value;
    }
  }

  auto texture = new Texture(path);
  glGenTextures(1, &texture->id_);
  stbi_set_flip_vertically_on_load(1);
  unsigned char* data =
      stbi_load(path.data(), &texture->width_, &texture->height_, &texture->channels_, 0);
  if (data == nullptr) {
    IDebug::Throw(std::format("Failed to load texture. Details: {}", path));
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
  g_loaded_textures[std::string(path)] = texture;
  return texture;
}