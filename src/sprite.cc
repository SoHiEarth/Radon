#include <classes/material.h>
#include <classes/sprite.h>
#include <engine/filesystem.h>
#include <engine/render.h>
#include <fmt/core.h>

void Sprite::Init() {
}

void Sprite::Update() {
  if (material_ != nullptr) {
    if (material_->shininess_ <= 0) {
      material_->shininess_ = 1;
    }
  }
}

void Sprite::Render() {
  render::RenderTexture(material_, static_cast<glm::vec3>(position_),
                        static_cast<glm::vec2>(scale_), static_cast<float>(rotation_));
}

void Sprite::Quit() {
  filesystem::FreeMaterial(material_);
}

void Sprite::Load(pugi::xml_node& node) {
  filesystem::serialized::LoadEditable(&position_, node);
  filesystem::serialized::LoadEditable(&rotation_, node);
  filesystem::serialized::LoadEditable(&scale_, node);
  filesystem::serialized::LoadMaterial(material_, node);
}
void Sprite::Save(pugi::xml_node& node) const {
  filesystem::serialized::SaveEditable(position_, node);
  filesystem::serialized::SaveEditable(rotation_, node);
  filesystem::serialized::SaveEditable(scale_, node);
  filesystem::serialized::SaveMaterial(material_, node);
}
