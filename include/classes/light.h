#ifndef LIGHT_H
#define LIGHT_H

#include <classes/object.h>
#include <glm/glm.hpp>

class Shader;
class Light : public Object {
public:
  Editable<glm::vec3> ambient_ = {glm::vec3(0.0F), "Ambient", reg_},
                      diffuse_ = {glm::vec3(0.0F), "Diffuse", reg_},
                      specular_ = {glm::vec3(0.0F), "Specular", reg_};
  void Update() override {};
  void Render() override {};
};

class DirectionalLight : public Light {
public:
  Editable<glm::vec3> direction_ = {glm::vec3(0.0F), "Direction", reg_};
  void Init() override;
  void Quit() override;
  void SetUniforms(const Shader* shader);
};

class PointLight : public Light {
public:
  Editable<float> constant_ = {1.0F, "Constant", reg_}, linear_ = {0.0F, "Linear", reg_},
                  quadratic_ = {0.0F, "Quadratic", reg_};
  void Init() override;
  void Quit() override;
  void SetUniforms(const Shader* shader, int k_pos);
};

class SpotLight : public Light {
public:
  Editable<glm::vec3> direction_ = {glm::vec3(0.0F), "Direction", reg_};
  Editable<float> constant_ = {1.0F, "Constant", reg_}, linear_ = {0.0F, "Linear", reg_},
                  quadratic_ = {0.0F, "Quadratic", reg_}, cut_off_ = {20.0F, "Cutoff", reg_},
                  outer_cut_off_ = {25.0F, "Outer Cutoff", reg_};
  void Init() override;
  void Quit() override;
  void SetUniforms(const Shader* shader, int k_pos);
};

#endif  // LIGHT_H
