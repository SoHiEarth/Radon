#include <classes/material.h>
#include <classes/meshrenderer.h>
#include <classes/object.h>
#include <engine/debug.h>
#include <engine/io.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <fmt/core.h>

void MeshRenderer::Update() {
  if (material_ != nullptr) {
    if (material_->shininess_ <= 0) {
      material_->shininess_ = 1;
    }
  }
}

void MeshRenderer::Render() {
  if (auto parent = parent_.lock()) {
    render::RenderTexture(material_, static_cast<glm::vec3>(parent->transform_.position_),
                          static_cast<glm::vec2>(parent->transform_.scale_),
                          static_cast<glm::vec3>(parent->transform_.rotation_));
  }
}

void MeshRenderer::Load(pugi::xml_node& node) {
  material_ = io::xml::LoadMaterial(node);
}
void MeshRenderer::Save(pugi::xml_node& node) const {
  io::xml::SaveMaterial(*material_, node);
}
