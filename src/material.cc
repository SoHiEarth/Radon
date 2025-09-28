#include <classes/material.h>
#include <classes/shader.h>
#include <classes/texture.h>
#include <engine/io.h>
#include <glad/glad.h>

bool Material::IsValid() const {
  return ((diffuse_ != nullptr) && (specular_ != nullptr) && (shader_ != nullptr));
}

void Material::Bind() const {
  if (!IsValid()) {
    return;
  }
  shader_->Use();
}
