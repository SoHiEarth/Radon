#include <classes/material.h>
#include <classes/modelrenderer.h>
#include <classes/object.h>
#include <engine/debug.h>
#include <engine/io.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <fmt/core.h>

void ModelRenderer::Render() {
  if (material_ != nullptr) {
    engine_->GetRenderer().DrawModel(model_, material_->shader_.get(),
                                          static_cast<glm::vec3>(parent_->transform_.position_),
                                          static_cast<glm::vec2>(parent_->transform_.scale_),
                                          static_cast<glm::vec3>(parent_->transform_.rotation_));
  }
}

void ModelRenderer::Load(pugi::xml_node& node) {
  auto& io = engine_->GetIO();
  model_ = io.LoadModel(node);
  material_ = io.LoadMaterial(node);
}
void ModelRenderer::Save(pugi::xml_node& node) const {
  auto& io = engine_->GetIO();
  io.SaveModel(model_, node);
  io.SaveMaterial(material_, node);
}