#include <classes/material.h>
#include <classes/shader.h>
#include <classes/texture.h>
#include <engine/filesystem.h>
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
  diffuse_->Use();
  glActiveTexture(GL_TEXTURE1);
  specular_->Use();
}

void Material::Load(const std::string& path) {
  directory_ = path;
  diffuse_ = f::LoadTexture(path + "/diffuse.png");
  specular_ = f::LoadTexture(path + "/specular.png");
  shader_ = f::LoadShader(path + "/vert.glsl", path + "/frag.glsl");
  if (!IsValid()) {
    return;
  }
  shader_->Use();
  shader_->SetInt("material.diffuse", 0);
  shader_->SetInt("material.specular", 1);
}

void Material::Free() const {
  f::FreeTexture(diffuse_);
  f::FreeTexture(specular_);
  f::FreeShader(shader_);
}
