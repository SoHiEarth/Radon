#include <classes/material.h>
#include <classes/sprite.h>
#include <engine/debug.h>
#include <engine/io.h>
#include <engine/render.h>
#include <fmt/core.h>

void Sprite::Init() {
  if (material_ == nullptr) {
    debug::Log(GET_TRACE, "No material assigned to sprite.");
  }
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
                        static_cast<glm::vec2>(scale_), static_cast<glm::vec3>(rotation_));
}

void Sprite::Quit() {
  io::FreeMaterial(material_);
}

void Sprite::Load(pugi::xml_node& node) {
  name_ = io::serialized::LoadString(node, name_.i_label_);
  position_ = io::serialized::LoadVec3(node, position_.i_label_);
  rotation_ = io::serialized::LoadVec3(node, rotation_.i_label_);
  scale_ = io::serialized::LoadVec2(node, scale_.i_label_);
  material_ = io::serialized::LoadMaterial(node);
}
void Sprite::Save(pugi::xml_node& node) const {
  io::serialized::SaveString(&name_.i_value_, node, name_.i_label_);
  io::serialized::SaveVec3(&position_.i_value_, node, position_.i_label_);
  io::serialized::SaveVec3(&rotation_.i_value_, node, rotation_.i_label_);
  io::serialized::SaveVec2(&scale_.i_value_, node, scale_.i_label_);
  io::serialized::SaveMaterial(material_, node);
}