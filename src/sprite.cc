#include <classes/material.h>
#include <classes/sprite.h>
#include <engine/filesystem.h>
#include <engine/render.h>
#include <fmt/core.h>

void Sprite::Init() {
  material_ = new Material(static_cast<std::string>(path_));
  prev_path_ = static_cast<std::string>(path_);
}

void Sprite::Update() {
  if (material_ != nullptr) {
    if (material_->shininess_ <= 0) {
      material_->shininess_ = 1;
    }
  }
  if (path_ != prev_path_) {
    material_->Free();
    material_->Load(static_cast<std::string>(path_));
    prev_path_ = static_cast<std::string>(path_);
  }
}

void Sprite::Render() {
  render::RenderTexture(material_, static_cast<glm::vec3>(position_),
                        static_cast<glm::vec2>(scale_), static_cast<float>(rotation_));
}

void Sprite::Quit() {
  material_->Free();
}

void Sprite::Load(pugi::xml_node& node) {
  filesystem::LoadEditableSerialized(&position_, node);
  filesystem::LoadEditableSerialized(&rotation_, node);
  filesystem::LoadEditableSerialized(&scale_, node);
  filesystem::LoadEditableSerialized(&path_, node);
}
void Sprite::Save(pugi::xml_node& node) const {
  filesystem::SaveEditableSerialized(position_, node);
  filesystem::SaveEditableSerialized(rotation_, node);
  filesystem::SaveEditableSerialized(scale_, node);
  filesystem::SaveEditableSerialized(path_, node);
}
