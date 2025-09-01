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
  filesystem::LoadEditableSerialized(&direction_, node);
  filesystem::LoadEditableSerialized(&ambient_, node);
  filesystem::LoadEditableSerialized(&diffuse_, node);
  filesystem::LoadEditableSerialized(&specular_, node);
}
void DirectionalLight::Save(pugi::xml_node& node) const {
  filesystem::SaveEditableSerialized(direction_, node);
  filesystem::SaveEditableSerialized(ambient_, node);
  filesystem::SaveEditableSerialized(diffuse_, node);
  filesystem::SaveEditableSerialized(specular_, node);
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
  filesystem::LoadEditableSerialized(&position_, node);
  filesystem::LoadEditableSerialized(&ambient_, node);
  filesystem::LoadEditableSerialized(&diffuse_, node);
  filesystem::LoadEditableSerialized(&specular_, node);
  filesystem::LoadEditableSerialized(&constant_, node);
  filesystem::LoadEditableSerialized(&linear_, node);
  filesystem::LoadEditableSerialized(&quadratic_, node);
}
void PointLight::Save(pugi::xml_node& node) const {
  filesystem::SaveEditableSerialized(position_, node);
  filesystem::SaveEditableSerialized(ambient_, node);
  filesystem::SaveEditableSerialized(diffuse_, node);
  filesystem::SaveEditableSerialized(specular_, node);
  filesystem::SaveEditableSerialized(constant_, node);
  filesystem::SaveEditableSerialized(linear_, node);
  filesystem::SaveEditableSerialized(quadratic_, node);
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
  filesystem::LoadEditableSerialized(&position_, node);
  filesystem::LoadEditableSerialized(&direction_, node);
  filesystem::LoadEditableSerialized(&ambient_, node);
  filesystem::LoadEditableSerialized(&diffuse_, node);
  filesystem::LoadEditableSerialized(&specular_, node);
  filesystem::LoadEditableSerialized(&constant_, node);
  filesystem::LoadEditableSerialized(&linear_, node);
  filesystem::LoadEditableSerialized(&quadratic_, node);
  filesystem::LoadEditableSerialized(&cut_off_, node);
  filesystem::LoadEditableSerialized(&outer_cut_off_, node);
}
void SpotLight::Save(pugi::xml_node& node) const {
  filesystem::SaveEditableSerialized(position_, node);
  filesystem::SaveEditableSerialized(direction_, node);
  filesystem::SaveEditableSerialized(ambient_, node);
  filesystem::SaveEditableSerialized(diffuse_, node);
  filesystem::SaveEditableSerialized(specular_, node);
  filesystem::SaveEditableSerialized(constant_, node);
  filesystem::SaveEditableSerialized(linear_, node);
  filesystem::SaveEditableSerialized(quadratic_, node);
  filesystem::SaveEditableSerialized(cut_off_, node);
  filesystem::SaveEditableSerialized(outer_cut_off_, node);
}

void SpotLight::SetUniforms(const Shader* shader, const int kPos) {
  std::string prefix = "spot_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3(prefix + ".position", static_cast<glm::vec3>(position_));
  shader->SetVec3(prefix + ".direction", static_cast<glm::vec3>(direction_));
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
