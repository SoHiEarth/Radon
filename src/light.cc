#include <classes/light.h>
#include <classes/material.h>
#include <classes/shader.h>
#include <engine/filesystem.h>
#include <engine/render.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <glm/fwd.hpp>

void DirectionalLight::Init() {
  render::AddLight(this);
}

void DirectionalLight::Quit() {
  render::RemoveLight(this);
}

void DirectionalLight::Load(pugi::xml_node& node) {
  name_ = filesystem::serialized::LoadString(node, name_.i_label_);
  direction_ = filesystem::serialized::LoadVec3(node, direction_.i_label_);
  ambient_ = filesystem::serialized::LoadVec3(node, ambient_.i_label_);
  diffuse_ = filesystem::serialized::LoadVec3(node, diffuse_.i_label_);
  specular_ = filesystem::serialized::LoadVec3(node, specular_.i_label_);
}

void DirectionalLight::Save(pugi::xml_node& node) const {
  filesystem::serialized::SaveString(&name_.i_value_, node, name_.i_label_);
  filesystem::serialized::SaveVec3(&direction_.i_value_, node, direction_.i_label_);
  filesystem::serialized::SaveVec3(&ambient_.i_value_, node, ambient_.i_label_);
  filesystem::serialized::SaveVec3(&diffuse_.i_value_, node, diffuse_.i_label_);
  filesystem::serialized::SaveVec3(&specular_.i_value_, node, specular_.i_label_);
}

void DirectionalLight::SetUniforms(const Shader* shader, const int kPos) {
  std::string prefix = "directional_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3(prefix + ".direction", static_cast<glm::vec3>(direction_));
  shader->SetVec3(prefix + ".ambient", static_cast<glm::vec3>(ambient_));
  shader->SetVec3(prefix + ".diffuse", static_cast<glm::vec3>(diffuse_));
  shader->SetVec3(prefix + ".specular", static_cast<glm::vec3>(specular_));
}

void PointLight::Init() {
  render::AddLight(this);
}

void PointLight::Quit() {
  render::RemoveLight(this);
}

void PointLight::Load(pugi::xml_node& node) {
  name_ = filesystem::serialized::LoadString(node, name_.i_label_);
  position_ = filesystem::serialized::LoadVec3(node, position_.i_label_);
  ambient_ = filesystem::serialized::LoadVec3(node, ambient_.i_label_);
  diffuse_ = filesystem::serialized::LoadVec3(node, diffuse_.i_label_);
  specular_ = filesystem::serialized::LoadVec3(node, specular_.i_label_);
  constant_ = filesystem::serialized::LoadFloat(node, constant_.i_label_);
  linear_ = filesystem::serialized::LoadFloat(node, linear_.i_label_);
  quadratic_ = filesystem::serialized::LoadFloat(node, quadratic_.i_label_);
}
void PointLight::Save(pugi::xml_node& node) const {
  filesystem::serialized::SaveString(&name_.i_value_, node, name_.i_label_);
  filesystem::serialized::SaveVec3(&position_.i_value_, node, position_.i_label_);
  filesystem::serialized::SaveVec3(&ambient_.i_value_, node, ambient_.i_label_);
  filesystem::serialized::SaveVec3(&diffuse_.i_value_, node, diffuse_.i_label_);
  filesystem::serialized::SaveVec3(&specular_.i_value_, node, specular_.i_label_);
  filesystem::serialized::SaveFloat(&constant_.i_value_, node, constant_.i_label_);
  filesystem::serialized::SaveFloat(&linear_.i_value_, node, linear_.i_label_);
  filesystem::serialized::SaveFloat(&quadratic_.i_value_, node, quadratic_.i_label_);
}

void PointLight::SetUniforms(const Shader* shader, const int kPos) {
  std::string prefix = "point_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3(prefix + ".position", static_cast<glm::vec3>(position_));
  shader->SetVec3(prefix + ".ambient", static_cast<glm::vec3>(ambient_));
  shader->SetVec3(prefix + ".diffuse", static_cast<glm::vec3>(diffuse_));
  shader->SetVec3(prefix + ".specular", static_cast<glm::vec3>(specular_));
  shader->SetFloat(prefix + ".constant", static_cast<float>(constant_));
  shader->SetFloat(prefix + ".linear", static_cast<float>(linear_));
  shader->SetFloat(prefix + ".quadratic", static_cast<float>(quadratic_));
}

void SpotLight::Init() {
  render::AddLight(this);
}

void SpotLight::Quit() {
  render::RemoveLight(this);
}

void SpotLight::Load(pugi::xml_node& node) {
  name_ = filesystem::serialized::LoadString(node, name_.i_label_);
  position_ = filesystem::serialized::LoadVec3(node, position_.i_label_);
  rotation_ = filesystem::serialized::LoadVec3(node, rotation_.i_label_);
  ambient_ = filesystem::serialized::LoadVec3(node, ambient_.i_label_);
  diffuse_ = filesystem::serialized::LoadVec3(node, diffuse_.i_label_);
  specular_ = filesystem::serialized::LoadVec3(node, specular_.i_label_);
  constant_ = filesystem::serialized::LoadFloat(node, constant_.i_label_);
  linear_ = filesystem::serialized::LoadFloat(node, linear_.i_label_);
  quadratic_ = filesystem::serialized::LoadFloat(node, quadratic_.i_label_);
  cut_off_ = filesystem::serialized::LoadFloat(node, cut_off_.i_label_);
  outer_cut_off_ = filesystem::serialized::LoadFloat(node, outer_cut_off_.i_label_);
}

void SpotLight::Save(pugi::xml_node& node) const {
  filesystem::serialized::SaveString(&name_.i_value_, node, name_.i_label_);
  filesystem::serialized::SaveVec3(&position_.i_value_, node, position_.i_label_);
  filesystem::serialized::SaveVec3(&rotation_.i_value_, node, rotation_.i_label_);
  filesystem::serialized::SaveVec3(&ambient_.i_value_, node, ambient_.i_label_);
  filesystem::serialized::SaveVec3(&diffuse_.i_value_, node, diffuse_.i_label_);
  filesystem::serialized::SaveVec3(&specular_.i_value_, node, specular_.i_label_);
  filesystem::serialized::SaveFloat(&constant_.i_value_, node, constant_.i_label_);
  filesystem::serialized::SaveFloat(&linear_.i_value_, node, linear_.i_label_);
  filesystem::serialized::SaveFloat(&quadratic_.i_value_, node, quadratic_.i_label_);
  filesystem::serialized::SaveFloat(&cut_off_.i_value_, node, cut_off_.i_label_);
  filesystem::serialized::SaveFloat(&outer_cut_off_.i_value_, node, outer_cut_off_.i_label_);
}

void SpotLight::SetUniforms(const Shader* shader, const int kPos) {
  std::string prefix = "spot_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3(prefix + ".position", static_cast<glm::vec3>(position_));
  shader->SetVec3(prefix + ".direction", static_cast<glm::vec3>(rotation_));
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
