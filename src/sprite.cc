#include <classes/material.h>
#include <classes/sprite.h>
#include <engine/render.h>
#include <fmt/core.h>

void Sprite::Init() {
  material_ = new Material(static_cast<std::string>(path_));
  prev_path_ = static_cast<std::string>(path_);
}

void Sprite::Update() {
  if (path_ != prev_path_) {
    material_->Free();
    material_->Load(static_cast<std::string>(path_));
    prev_path_ = static_cast<std::string>(path_);
  }
}

void Sprite::Render() {
  r::RenderTexture(material_, static_cast<glm::vec3>(position_), static_cast<glm::vec2>(scale_),
                   static_cast<float>(rotation_));
}

void Sprite::Quit() {
  material_->Free();
}

void Sprite::Load(pugi::xml_node& node) {}
void Sprite::Save(pugi::xml_node& node) const {}
