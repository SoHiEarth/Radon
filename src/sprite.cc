#include <classes/material.h>
#include <classes/sprite.h>
#include <engine/debug.h>
#include <engine/io.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <classes/object.h>
#include <fmt/core.h>

void Sprite::Init() {
  if (material_ == nullptr) {
    debug::Log("No material assigned to sprite.");
  }
  // Create body
  /* physics_body_ =
      physics::CreateBody(glm::vec2(position_->x, position_->y), static_cast<glm::vec2>(scale_));
  * To be moved to physics component */
}

void Sprite::Update() {
  if (material_ != nullptr) {
    if (material_->shininess_ <= 0) {
      material_->shininess_ = 1;
    }
  }

  /*
  if (!is_static_) {
    const glm::vec2 kPhysPos = physics::GetBodyPosition(physics_body_);
    position_ = glm::vec3(kPhysPos, position_->z);
  }
  */
}

void Sprite::Render() {
  if (auto parent = parent_.lock()) {
    render::RenderTexture(material_, static_cast<glm::vec3>(parent->position_),
                          static_cast<glm::vec2>(parent->scale_),
                          static_cast<glm::vec3>(parent->rotation_));
  }
}

void Sprite::Quit() {
}

void Sprite::Load(pugi::xml_node& node) {
  material_ = io::xml::LoadMaterial(node);
}
void Sprite::Save(pugi::xml_node& node) const {
  io::xml::SaveMaterial(*material_, node);
}
