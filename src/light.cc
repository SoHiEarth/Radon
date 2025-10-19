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
  engine_->GetRenderer().AddLight(this);
}

void DirectionalLight::Quit() {
  engine_->GetRenderer().RemoveLight(this);
}

void DirectionalLight::Load(pugi::xml_node& node) {
  auto& io = engine_->GetIO();
  ambient_ = io.Load<glm::vec3>(node, ambient_.i_label_);
  diffuse_ = io.Load<glm::vec3>(node, diffuse_.i_label_);
  specular_ = io.Load<glm::vec3>(node, specular_.i_label_);
}

void DirectionalLight::Save(pugi::xml_node& node) const {
  auto& io = engine_->GetIO();
  io.Save(ambient_.i_value_, node, ambient_.i_label_);
  io.Save(diffuse_.i_value_, node, diffuse_.i_label_);
  io.Save(specular_.i_value_, node, specular_.i_label_);
}

void DirectionalLight::SetUniforms(const Shader* shader, const int kPos) {
  const auto prefix = "directional_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3((prefix + ".direction").c_str(), parent_->transform_.position_);
  shader->SetVec3((prefix + ".ambient").c_str(), ambient_);
  shader->SetVec3((prefix + ".diffuse").c_str(), diffuse_);
  shader->SetVec3((prefix + ".specular").c_str(), specular_);
}

void PointLight::Init() {
  engine_->GetRenderer().AddLight(this);
}

void PointLight::Quit() {
  engine_->GetRenderer().RemoveLight(this);
}

void PointLight::Load(pugi::xml_node& node) {
  auto& io = engine_->GetIO();
  ambient_ = io.Load<glm::vec3>(node, ambient_.i_label_);
  diffuse_ = io.Load<glm::vec3>(node, diffuse_.i_label_);
  specular_ = io.Load<glm::vec3>(node, specular_.i_label_);
  constant_ = io.Load<float>(node, constant_.i_label_);
  linear_ = io.Load<float>(node, linear_.i_label_);
  quadratic_ = io.Load<float>(node, quadratic_.i_label_);
}
void PointLight::Save(pugi::xml_node& node) const {
  auto& io = engine_->GetIO();
  io.Save(ambient_.i_value_, node, ambient_.i_label_);
  io.Save(diffuse_.i_value_, node, diffuse_.i_label_);
  io.Save(specular_.i_value_, node, specular_.i_label_);
  io.Save(constant_.i_value_, node, constant_.i_label_);
  io.Save(linear_.i_value_, node, linear_.i_label_);
  io.Save(quadratic_.i_value_, node, quadratic_.i_label_);
}

void PointLight::SetUniforms(const Shader* shader, const int kPos) {
  const auto prefix = "point_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3((prefix + ".position").c_str(), parent_->transform_.position_);
  shader->SetVec3((prefix + ".ambient").c_str(), ambient_);
  shader->SetVec3((prefix + ".diffuse").c_str(), diffuse_);
  shader->SetVec3((prefix + ".specular").c_str(), specular_);
  shader->SetFloat((prefix + ".constant").c_str(), constant_);
  shader->SetFloat((prefix + ".linear").c_str(), linear_);
  shader->SetFloat((prefix + ".quadratic").c_str(), quadratic_);
}

void SpotLight::Init() {
  engine_->GetRenderer().AddLight(this);
}

void SpotLight::Quit() {
  engine_->GetRenderer().RemoveLight(this);
}

void SpotLight::Load(pugi::xml_node& node) {
  auto& io = engine_->GetIO();
  ambient_ = io.Load<glm::vec3>(node, ambient_.i_label_);
  diffuse_ = io.Load<glm::vec3>(node, diffuse_.i_label_);
  specular_ = io.Load<glm::vec3>(node, specular_.i_label_);
  constant_ = io.Load<float>(node, constant_.i_label_);
  linear_ = io.Load<float>(node, linear_.i_label_);
  quadratic_ = io.Load<float>(node, quadratic_.i_label_);
  cut_off_ = io.Load<float>(node, cut_off_.i_label_);
  outer_cut_off_ = io.Load<float>(node, outer_cut_off_.i_label_);
}

void SpotLight::Save(pugi::xml_node& node) const {
  auto& io = engine_->GetIO();
  io.Save(ambient_.i_value_, node, ambient_.i_label_);
  io.Save(diffuse_.i_value_, node, diffuse_.i_label_);
  io.Save(specular_.i_value_, node, specular_.i_label_);
  io.Save(constant_.i_value_, node, constant_.i_label_);
  io.Save(linear_.i_value_, node, linear_.i_label_);
  io.Save(quadratic_.i_value_, node, quadratic_.i_label_);
  io.Save(cut_off_.i_value_, node, cut_off_.i_label_);
  io.Save(outer_cut_off_.i_value_, node, outer_cut_off_.i_label_);
}

void SpotLight::SetUniforms(const Shader* shader, const int kPos) {
  const auto prefix = "spot_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3((prefix + ".position").c_str(), parent_->transform_.position_);
  shader->SetVec3((prefix + ".direction").c_str(), parent_->transform_.rotation_);
  shader->SetVec3((prefix + ".ambient").c_str(), ambient_);
  shader->SetVec3((prefix + ".diffuse").c_str(), diffuse_);
  shader->SetVec3((prefix + ".specular").c_str(), specular_);
  shader->SetFloat((prefix + ".constant").c_str(), constant_);
  shader->SetFloat((prefix + ".linear").c_str(), linear_);
  shader->SetFloat((prefix + ".quadratic").c_str(), quadratic_);
  shader->SetFloat((prefix + ".cutoff").c_str(), glm::cos(glm::radians((float) cut_off_)));
  shader->SetFloat((prefix + ".outerCutoff").c_str(),
                   glm::cos(glm::radians((float) outer_cut_off_)));
}
