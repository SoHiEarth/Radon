import metal.material;
import metal.shader;
import metal.texture;
import metal.io;
#include <glad/glad.h>

bool Material::IsValid() const {
  return ((diffuse_ != nullptr) && (specular_ != nullptr) && (shader_ != nullptr));
}

void Material::Bind() const {
  if (!IsValid()) {
    return;
  }
  shader_->Use();
  shader_->SetFloat("material.shininess", shininess_);
  shader_->SetInt("material.diffuse", 0);
  shader_->SetInt("material.specular", 1);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, diffuse_->id_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, specular_->id_);
}
