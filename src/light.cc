#include <classes/light.h>
#include <classes/material.h>
#include <classes/shader.h>
#include <engine/io.h>
#include <engine/render.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <classes/object.h>
#include <classes/component.h>
#include <memory>

void DirectionalLight::Init() {
  render::AddLight(std::static_pointer_cast<DirectionalLight>(shared_from_this()));
}

void DirectionalLight::Quit() {
  render::RemoveLight(std::static_pointer_cast<DirectionalLight>(shared_from_this()));
}

void DirectionalLight::Load(pugi::xml_node& node) {
  ambient_ = io::xml::LoadVec3(node, ambient_.i_label_);
  diffuse_ = io::xml::LoadVec3(node, diffuse_.i_label_);
  specular_ = io::xml::LoadVec3(node, specular_.i_label_);
}

void DirectionalLight::Save(pugi::xml_node& node) const {
  io::xml::SaveVec3(ambient_.i_value_, node, ambient_.i_label_);
  io::xml::SaveVec3(diffuse_.i_value_, node, diffuse_.i_label_);
  io::xml::SaveVec3(specular_.i_value_, node, specular_.i_label_);
}

void DirectionalLight::SetUniforms(const std::unique_ptr<Shader>& shader, const int kPos) {
  std::string prefix = "directional_lights[" + std::to_string(kPos) + "]";
  if (auto parent = parent_.lock()) {
  shader->SetVec3(prefix + ".direction", static_cast<glm::vec3>(parent->rotation_));
  }
  shader->SetVec3(prefix + ".ambient", static_cast<glm::vec3>(ambient_));
  shader->SetVec3(prefix + ".diffuse", static_cast<glm::vec3>(diffuse_));
  shader->SetVec3(prefix + ".specular", static_cast<glm::vec3>(specular_));
}

void PointLight::Init() {
  render::AddLight(std::static_pointer_cast<PointLight>(shared_from_this()));
}

void PointLight::Quit() {
  render::RemoveLight(std::static_pointer_cast<PointLight>(shared_from_this()));
}

void PointLight::Load(pugi::xml_node& node) {
  ambient_ = io::xml::LoadVec3(node, ambient_.i_label_);
  diffuse_ = io::xml::LoadVec3(node, diffuse_.i_label_);
  specular_ = io::xml::LoadVec3(node, specular_.i_label_);
  constant_ = io::xml::LoadFloat(node, constant_.i_label_);
  linear_ = io::xml::LoadFloat(node, linear_.i_label_);
  quadratic_ = io::xml::LoadFloat(node, quadratic_.i_label_);
}
void PointLight::Save(pugi::xml_node& node) const {
  io::xml::SaveVec3(ambient_.i_value_, node, ambient_.i_label_);
  io::xml::SaveVec3(diffuse_.i_value_, node, diffuse_.i_label_);
  io::xml::SaveVec3(specular_.i_value_, node, specular_.i_label_);
  io::xml::SaveFloat(&constant_.i_value_, node, constant_.i_label_);
  io::xml::SaveFloat(&linear_.i_value_, node, linear_.i_label_);
  io::xml::SaveFloat(&quadratic_.i_value_, node, quadratic_.i_label_);
}

void PointLight::SetUniforms(const std::unique_ptr<Shader>& shader, const int kPos) {
  std::string prefix = "point_lights[" + std::to_string(kPos) + "]";
  if (auto parent = parent_.lock()) {
    shader->SetVec3(prefix + ".position", static_cast<glm::vec3>(parent->position_));
  }
  shader->SetVec3(prefix + ".ambient", static_cast<glm::vec3>(ambient_));
  shader->SetVec3(prefix + ".diffuse", static_cast<glm::vec3>(diffuse_));
  shader->SetVec3(prefix + ".specular", static_cast<glm::vec3>(specular_));
  shader->SetFloat(prefix + ".constant", static_cast<float>(constant_));
  shader->SetFloat(prefix + ".linear", static_cast<float>(linear_));
  shader->SetFloat(prefix + ".quadratic", static_cast<float>(quadratic_));
}

void SpotLight::Init() {
  render::AddLight(std::static_pointer_cast<SpotLight>(shared_from_this()));
}

void SpotLight::Quit() {
  render::RemoveLight(std::static_pointer_cast<SpotLight>(shared_from_this()));
}

void SpotLight::Load(pugi::xml_node& node) {
  ambient_ = io::xml::LoadVec3(node, ambient_.i_label_);
  diffuse_ = io::xml::LoadVec3(node, diffuse_.i_label_);
  specular_ = io::xml::LoadVec3(node, specular_.i_label_);
  constant_ = io::xml::LoadFloat(node, constant_.i_label_);
  linear_ = io::xml::LoadFloat(node, linear_.i_label_);
  quadratic_ = io::xml::LoadFloat(node, quadratic_.i_label_);
  cut_off_ = io::xml::LoadFloat(node, cut_off_.i_label_);
  outer_cut_off_ = io::xml::LoadFloat(node, outer_cut_off_.i_label_);
}

void SpotLight::Save(pugi::xml_node& node) const {
  io::xml::SaveVec3(ambient_.i_value_, node, ambient_.i_label_);
  io::xml::SaveVec3(diffuse_.i_value_, node, diffuse_.i_label_);
  io::xml::SaveVec3(specular_.i_value_, node, specular_.i_label_);
  io::xml::SaveFloat(&constant_.i_value_, node, constant_.i_label_);
  io::xml::SaveFloat(&linear_.i_value_, node, linear_.i_label_);
  io::xml::SaveFloat(&quadratic_.i_value_, node, quadratic_.i_label_);
  io::xml::SaveFloat(&cut_off_.i_value_, node, cut_off_.i_label_);
  io::xml::SaveFloat(&outer_cut_off_.i_value_, node, outer_cut_off_.i_label_);
}

void SpotLight::SetUniforms(const std::unique_ptr<Shader>& shader, const int kPos) {
  std::string prefix = "spot_lights[" + std::to_string(kPos) + "]";
  if (auto parent = parent_.lock()) {
    shader->SetVec3(prefix + ".position", static_cast<glm::vec3>(parent->position_));
    shader->SetVec3(prefix + ".direction", static_cast<glm::vec3>(parent->rotation_));
  }
  shader->SetVec3(prefix + ".ambient", static_cast<glm::vec3>(ambient_));
  shader->SetVec3(prefix + ".diffuse", static_cast<glm::vec3>(diffuse_));
  shader->SetVec3(prefix + ".specular", static_cast<glm::vec3>(specular_));
  shader->SetFloat(prefix + ".constant", static_cast<float>(constant_));
  shader->SetFloat(prefix + ".linear", static_cast<float>(linear_));
  shader->SetFloat(prefix + ".quadratic", static_cast<float>(quadratic_));
  shader->SetFloat(prefix + ".cutoff", glm::cos(glm::radians(static_cast<float>(cut_off_))));
  shader->SetFloat(prefix + ".outerCutoff",
                   glm::cos(glm::radians(static_cast<float>(outer_cut_off_))));
}
