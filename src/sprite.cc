#include <classes/sprite.h>
#include <engine/filesystem.h>
#include <engine/render.h>
#include <fmt/core.h>

void Sprite::Init() {
  texture = f::LoadTexture(path);
  shader = f::LoadShader("sprite_shader");
  prev_path = path;
}

void Sprite::Update() {
  if (path != prev_path) {
    f::FreeTexture(texture);
    texture = f::LoadTexture(path);
    prev_path = path;
  }
}

void Sprite::Render() {
  r::RenderTexture(texture, shader, position, scale, rotation);
}

void Sprite::Quit() {
  f::FreeTexture(texture);
}
