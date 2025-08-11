#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <classes/object.h>
class Shader;

class Light : public Object {
 public:
  bool active = false;
  glm::vec3 ambient = glm::vec3(0.0f),
    diffuse = glm::vec3(0.0f),
    specular = glm::vec3(0.0f);
  void Update() override {}; // Light update is done elsewhere
  void Render() override {};
};

class DirectionalLight : public Light {
 public:
  glm::vec3 direction = glm::vec3(0.0f);
  void Init() override;
  void Quit() override;
  void SetUniforms(const Shader* shader);
};

class PointLight : public Light {
 public:
  float constant = 1.0f,
    linear = 0.0f,
    quadratic = 0.0f;
  void Init() override;
  void Quit() override;
  void SetUniforms(const Shader* shader, const int pos);
};

class SpotLight : public Light {
 public:
  glm::vec3 direction = glm::vec3(0.0f);
  float constant = 1.0f,
    linear = 0.0f,
    quadratic = 0.0f,
    cutOff = 20.0f,
    outerCutOff = 25.0f;
  void Init() override;
  void Quit() override;
  void SetUniforms(const Shader* shader, const int pos);
};

#endif // LIGHT_H
