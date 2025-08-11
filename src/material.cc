#include <classes/material.h>
#include <classes/texture.h>
#include <classes/shader.h>
#include <engine/filesystem.h>
#include <glad/glad.h>

const bool Material::IsValid() const {
  return (diffuse && specular && shader);
}

void Material::Bind() const {
  if (!IsValid()) return;
  shader->Use();
  shader->SetFloat("material.shininess", shininess);
  shader->SetInt("material.diffuse", 0);
  shader->SetInt("material.specular", 1);
  glActiveTexture(GL_TEXTURE0);
  diffuse->Use();
  glActiveTexture(GL_TEXTURE1);
  specular->Use();
}

void Material::Load(const std::string& path) {
  directory = path;
  diffuse = f::LoadTexture(path + "/diffuse.png");
  specular = f::LoadTexture(path + "/specular.png");
  shader = f::LoadShader(path + "/vert.glsl", path + "/frag.glsl");
  if (!IsValid()) return;
  shader->Use();
  shader->SetInt("material.diffuse", 0);
  shader->SetInt("material.specular", 1);
}

void Material::Free() {
  f::FreeTexture(diffuse);
  f::FreeTexture(specular);
  f::FreeShader(shader);
}
