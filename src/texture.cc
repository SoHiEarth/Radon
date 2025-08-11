#include <classes/texture.h>
#include <glad/glad.h>

void Texture::Use() const {
  glBindTexture(GL_TEXTURE_2D, id);
}
