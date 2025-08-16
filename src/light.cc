#include <classes/light.h>
#include <classes/material.h>
#include <classes/shader.h>
#include <engine/filesystem.h>
#include <engine/render.h>
#include <fmt/core.h>
#include <glad/glad.h>
#include <glm/fwd.hpp>

void DirectionalLight::Init() {
  r::AddLight(this);
}

void DirectionalLight::Quit() {
  r::RemoveLight(this);
}

void DirectionalLight::Load(pugi::xml_node& node) {
  f::LoadEditableSerialized(&direction_, node);
  f::LoadEditableSerialized(&ambient_, node);
  f::LoadEditableSerialized(&diffuse_, node);
  f::LoadEditableSerialized(&specular_, node);
}
void DirectionalLight::Save(pugi::xml_node& node) const {
  f::SaveEditableSerialized(direction_, node);
  f::SaveEditableSerialized(ambient_, node);
  f::SaveEditableSerialized(diffuse_, node);
  f::SaveEditableSerialized(specular_, node);
}

void DirectionalLight::SetUniforms(const Shader* shader, const int kPos) {
  std::string prefix = "directional_lights[" + std::to_string(kPos) + "]";
  shader->SetVec3(prefix + ".direction", static_cast<glm::vec3>(direction_));
  shader->SetVec3(prefix + ".ambient", static_cast<glm::vec3>(ambient_));
  shader->SetVec3(prefix + ".diffuse", static_cast<glm::vec3>(diffuse_));
  shader->SetVec3(prefix + ".specular", static_cast<glm::vec3>(specular_));
}

void PointLight::Init() {
  r::AddLight(this);
}

void PointLight::Quit() {
  r::RemoveLight(this);
}

void PointLight::Load(pugi::xml_node& node) {
  f::LoadEditableSerialized(&position_, node);
  f::LoadEditableSerialized(&ambient_, node);
  f::LoadEditableSerialized(&diffuse_, node);
  f::LoadEditableSerialized(&specular_, node);
  f::LoadEditableSerialized(&constant_, node);
  f::LoadEditableSerialized(&linear_, node);
  f::LoadEditableSerialized(&quadratic_, node);
}
void PointLight::Save(pugi::xml_node& node) const {
  f::SaveEditableSerialized(position_, node);
  f::SaveEditableSerialized(ambient_, node);
  f::SaveEditableSerialized(diffuse_, node);
  f::SaveEditableSerialized(specular_, node);
  f::SaveEditableSerialized(constant_, node);
  f::SaveEditableSerialized(linear_, node);
  f::SaveEditableSerialized(quadratic_, node);
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
  r::AddLight(this);
}

void SpotLight::Quit() {
  r::RemoveLight(this);
}

void SpotLight::Load(pugi::xml_node& node) {
  f::LoadEditableSerialized(&position_, node);
  f::LoadEditableSerialized(&direction_, node);
  f::LoadEditableSerialized(&ambient_, node);
  f::LoadEditableSerialized(&diffuse_, node);
  f::LoadEditableSerialized(&specular_, node);
  f::LoadEditableSerialized(&constant_, node);
  f::LoadEditableSerialized(&linear_, node);
  f::LoadEditableSerialized(&quadratic_, node);
  f::LoadEditableSerialized(&cut_off_, node);
  f::LoadEditableSerialized(&outer_cut_off_, node);
}
void SpotLight::Save(pugi::xml_node& node) const {
  f::SaveEditableSerialized(position_, node);
  f::SaveEditableSerialized(direction_, node);
  f::SaveEditableSerialized(ambient_, node);
  f::SaveEditableSerialized(diffuse_, node);
  f::SaveEditableSerialized(specular_, node);
  f::SaveEditableSerialized(constant_, node);
  f::SaveEditableSerialized(linear_, node);
  f::SaveEditableSerialized(quadratic_, node);
  f::SaveEditableSerialized(cut_off_, node);
  f::SaveEditableSerialized(outer_cut_off_, node);
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
