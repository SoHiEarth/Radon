#include <classes/light.h>
#include <classes/material.h>
#include <classes/shader.h>
#include <engine/render.h>
#include <fmt/core.h>
#include <glad/glad.h>

void DirectionalLight::Init() {
  r::AddLight(this);
}

void DirectionalLight::Quit() {
  r::RemoveLight(this);
}

void DirectionalLight::Load(pugi::xml_node& node) {}
void DirectionalLight::Save(pugi::xml_node& node) const {}

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

void PointLight::Load(pugi::xml_node& node) {}
void PointLight::Save(pugi::xml_node& node) const {}

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

void SpotLight::Load(pugi::xml_node& node) {}
void SpotLight::Save(pugi::xml_node& node) const {}

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
