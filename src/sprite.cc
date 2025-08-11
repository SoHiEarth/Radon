#include <classes/sprite.h>
#include <engine/filesystem.h>
#include <engine/render.h>
#include <fmt/core.h>

void Sprite::Init() {
  material = new Material();
  material->diffuse = f::LoadTexture(path + "/diffuse.png");
  material->specular = f::LoadTexture(path + "/specular.png");
  shader = f::LoadShader("sprite_shader");
  prev_path = path;
}

void Sprite::Update() {
  if (path != prev_path) {
    f::FreeTexture(material->diffuse);
    f::FreeTexture(material->specular);
    material->diffuse = f::LoadTexture(path + "/diffuse.png");
    material->specular = f::LoadTexture(path + "/specular.png");
    prev_path = path;
  }
}

void Sprite::Render() {
  r::RenderTexture(material, shader, position, scale, rotation);
}

void Sprite::Quit() {
  f::FreeTexture(material->diffuse);
  f::FreeTexture(material->specular);
}
