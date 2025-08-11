#include <classes/sprite.h>
#include <classes/material.h>
#include <engine/render.h>
#include <fmt/core.h>

void Sprite::Init() {
  material = new Material(path);
  prev_path = path;
}

void Sprite::Update() {
  if (path != prev_path) {
    material->Free();
    material->Load(path);
    prev_path = path;
  }
}

void Sprite::Render() {
  r::RenderTexture(material, position, scale, rotation);
}

void Sprite::Quit() {
  material->Free();
}
