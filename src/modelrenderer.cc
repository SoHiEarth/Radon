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
    IRenderer::Get<IRenderer>().DrawModel(model_, material_->shader_,
                      static_cast<glm::vec3>(parent_->transform_.position_),
                      static_cast<glm::vec2>(parent_->transform_.scale_),
                      static_cast<glm::vec3>(parent_->transform_.rotation_));
  }
}

void ModelRenderer::Load(pugi::xml_node& node) {
  model_ = IIO::Get<IIO>().LoadModel(node);
  material_ = IIO::Get<IIO>().LoadMaterial(node);
}
void ModelRenderer::Save(pugi::xml_node& node) const {
  IIO::Get<IIO>().SaveModel(model_, node);
  IIO::Get<IIO>().SaveMaterial(material_, node);
}