#include <classes/component.h>
#include <classes/light.h>
#include <classes/material.h>
#include <classes/object.h>
#include <classes/shader.h>
#include <classes/transform.h>
#include <engine/io.h>
#include <engine/render.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <memory>

void DirectionalLight::Init() {
  IRenderer::Get<IRenderer>().GAddLight(this);
}

void DirectionalLight::Quit() {
  IRenderer::Get<IRenderer>().GRemoveLight(this);
}

void DirectionalLight::Load(pugi::xml_node& node) {
  ambient_ = IIO::Get<IIO>().LoadVec3(node, ambient_.i_label_);
  diffuse_ = IIO::Get<IIO>().LoadVec3(node, diffuse_.i_label_);
  specular_ = IIO::Get<IIO>().LoadVec3(node, specular_.i_label_);
}

void DirectionalLight::Save(pugi::xml_node& node) const {
  IIO::Get<IIO>().SaveVec3(ambient_.i_value_, node, ambient_.i_label_);
  IIO::Get<IIO>().SaveVec3(diffuse_.i_value_, node, diffuse_.i_label_);
  IIO::Get<IIO>().SaveVec3(specular_.i_value_, node, specular_.i_label_);
}

void DirectionalLight::SetUniforms(const Shader* shader, const int kPos) {
  std::string prefix = "directional_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3(prefix + ".direction", static_cast<glm::vec3>(parent_->transform_.position_));
  shader->SetVec3(prefix + ".ambient", static_cast<glm::vec3>(ambient_));
  shader->SetVec3(prefix + ".diffuse", static_cast<glm::vec3>(diffuse_));
  shader->SetVec3(prefix + ".specular", static_cast<glm::vec3>(specular_));
}

void PointLight::Init() {
  IRenderer::Get<IRenderer>().GAddLight(this);
}

void PointLight::Quit() {
  IRenderer::Get<IRenderer>().GRemoveLight(this);
}

void PointLight::Load(pugi::xml_node& node) {
  ambient_ = IIO::Get<IIO>().LoadVec3(node, ambient_.i_label_);
  diffuse_ = IIO::Get<IIO>().LoadVec3(node, diffuse_.i_label_);
  specular_ = IIO::Get<IIO>().LoadVec3(node, specular_.i_label_);
  constant_ = IIO::Get<IIO>().LoadFloat(node, constant_.i_label_);
  linear_ = IIO::Get<IIO>().LoadFloat(node, linear_.i_label_);
  quadratic_ = IIO::Get<IIO>().LoadFloat(node, quadratic_.i_label_);
}
void PointLight::Save(pugi::xml_node& node) const {
  IIO::Get<IIO>().SaveVec3(ambient_.i_value_, node, ambient_.i_label_);
  IIO::Get<IIO>().SaveVec3(diffuse_.i_value_, node, diffuse_.i_label_);
  IIO::Get<IIO>().SaveVec3(specular_.i_value_, node, specular_.i_label_);
  IIO::Get<IIO>().SaveFloat(&constant_.i_value_, node, constant_.i_label_);
  IIO::Get<IIO>().SaveFloat(&linear_.i_value_, node, linear_.i_label_);
  IIO::Get<IIO>().SaveFloat(&quadratic_.i_value_, node, quadratic_.i_label_);
}

void PointLight::SetUniforms(const Shader* shader, const int kPos) {
  std::string prefix = "point_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3(prefix + ".position", static_cast<glm::vec3>(parent_->transform_.position_));
  shader->SetVec3(prefix + ".ambient", static_cast<glm::vec3>(ambient_));
  shader->SetVec3(prefix + ".diffuse", static_cast<glm::vec3>(diffuse_));
  shader->SetVec3(prefix + ".specular", static_cast<glm::vec3>(specular_));
  shader->SetFloat(prefix + ".constant", static_cast<float>(constant_));
  shader->SetFloat(prefix + ".linear", static_cast<float>(linear_));
  shader->SetFloat(prefix + ".quadratic", static_cast<float>(quadratic_));
}

void SpotLight::Init() {
  IRenderer::Get<IRenderer>().GAddLight(this);
}

void SpotLight::Quit() {
  IRenderer::Get<IRenderer>().GRemoveLight(this);
}

void SpotLight::Load(pugi::xml_node& node) {
  ambient_ = IIO::Get<IIO>().LoadVec3(node, ambient_.i_label_);
  diffuse_ = IIO::Get<IIO>().LoadVec3(node, diffuse_.i_label_);
  specular_ = IIO::Get<IIO>().LoadVec3(node, specular_.i_label_);
  constant_ = IIO::Get<IIO>().LoadFloat(node, constant_.i_label_);
  linear_ = IIO::Get<IIO>().LoadFloat(node, linear_.i_label_);
  quadratic_ = IIO::Get<IIO>().LoadFloat(node, quadratic_.i_label_);
  cut_off_ = IIO::Get<IIO>().LoadFloat(node, cut_off_.i_label_);
  outer_cut_off_ = IIO::Get<IIO>().LoadFloat(node, outer_cut_off_.i_label_);
}

void SpotLight::Save(pugi::xml_node& node) const {
  IIO::Get<IIO>().SaveVec3(ambient_.i_value_, node, ambient_.i_label_);
  IIO::Get<IIO>().SaveVec3(diffuse_.i_value_, node, diffuse_.i_label_);
  IIO::Get<IIO>().SaveVec3(specular_.i_value_, node, specular_.i_label_);
  IIO::Get<IIO>().SaveFloat(&constant_.i_value_, node, constant_.i_label_);
  IIO::Get<IIO>().SaveFloat(&linear_.i_value_, node, linear_.i_label_);
  IIO::Get<IIO>().SaveFloat(&quadratic_.i_value_, node, quadratic_.i_label_);
  IIO::Get<IIO>().SaveFloat(&cut_off_.i_value_, node, cut_off_.i_label_);
  IIO::Get<IIO>().SaveFloat(&outer_cut_off_.i_value_, node, outer_cut_off_.i_label_);
}

void SpotLight::SetUniforms(const Shader* shader, const int kPos) {
  std::string prefix = "spot_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3(prefix + ".position", static_cast<glm::vec3>(parent_->transform_.position_));
  shader->SetVec3(prefix + ".direction", static_cast<glm::vec3>(parent_->transform_.rotation_));
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
