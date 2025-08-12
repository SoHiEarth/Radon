#include <classes/light.h>
#include <glad/glad.h>
#include <classes/shader.h>
#include <engine/render.h>
#include <classes/material.h>
#include <fmt/core.h>

void DirectionalLight::Init() {
  r::AddLight(this);
}

void DirectionalLight::Quit() {
  r::RemoveLight(this);
}

void DirectionalLight::SetUniforms(const Shader* shader) {
  shader->SetVec3("directional_light.direction", direction);
  shader->SetVec3("directional_light.ambient", ambient);
  shader->SetVec3("directional_light.diffuse", diffuse);
  shader->SetVec3("directional_light.specular", specular);
}

void PointLight::Init() {
  r::AddLight(this);
}

void PointLight::Quit() {
  r::RemoveLight(this);
}

void PointLight::SetUniforms(const Shader* shader, const int pos) {
  std::string prefix = "point_lights[" + std::to_string(pos) + "]";
  shader->SetVec3(prefix + ".position", position);
  shader->SetVec3(prefix + ".ambient", ambient);
  shader->SetVec3(prefix + ".diffuse", diffuse);
  shader->SetVec3(prefix + ".specular", specular);
  shader->SetFloat(prefix + ".constant", constant);
  shader->SetFloat(prefix + ".linear", linear);
  shader->SetFloat(prefix + ".quadratic", quadratic);
}

void SpotLight::Init() {
  r::AddLight(this);
}

void SpotLight::Quit() {
  r::RemoveLight(this);
}

void SpotLight::SetUniforms(const Shader* shader, const int pos) {
  std::string prefix = "spot_lights[" + std::to_string(pos) + "]";
  shader->SetVec3(prefix + ".position", position);
  shader->SetVec3(prefix + ".direction", direction);
  shader->SetVec3(prefix + ".ambient", ambient);
  shader->SetVec3(prefix + ".diffuse", diffuse);
  shader->SetVec3(prefix + ".specular", specular);
  shader->SetFloat(prefix + ".constant", constant);
  shader->SetFloat(prefix + ".linear", linear);
  shader->SetFloat(prefix + ".quadratic", quadratic);
  shader->SetFloat(prefix + "cutoff", glm::cos(glm::radians(cutOff.i_value)));
  shader->SetFloat(prefix + "outerCutoff", glm::cos(glm::radians(outerCutOff.i_value)));
}
