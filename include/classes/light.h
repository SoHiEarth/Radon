#ifndef LIGHT_H
#define LIGHT_H

#include <classes/component.h>
#include <glm/glm.hpp>
#include <memory>
#include <pugixml.hpp>

class Shader;
class Light : public Component {
public:
  Editable<glm::vec3> ambient_ = {glm::vec3(0.0F), "Ambient", reg_},
                      diffuse_ = {glm::vec3(0.0F), "Diffuse", reg_},
                      specular_ = {glm::vec3(0.0F), "Specular", reg_};
};

class DirectionalLight : public Light {
public:
  void Init() override;
  void Quit() override;
  void Load(pugi::xml_node&) override;
  [[nodiscard]] std::string GetTypeName() const override {
    return "DirectionalLight";
  }
  void Save(pugi::xml_node&) const override;
  void SetUniforms(const std::unique_ptr<Shader>&, int);
};

class PointLight : public Light {
public:
  Editable<float> constant_ = {1.0F, "Constant", reg_}, linear_ = {0.0F, "Linear", reg_},
                  quadratic_ = {0.0F, "Quadratic", reg_};
  void Init() override;
  void Quit() override;
  void Load(pugi::xml_node&) override;
  [[nodiscard]] std::string GetTypeName() const override {
    return "PointLight";
  }
  void Save(pugi::xml_node&) const override;
  void SetUniforms(const std::unique_ptr<Shader>&, int);
};

class SpotLight : public Light {
public:
  Editable<float> constant_ = {1.0F, "Constant", reg_}, linear_ = {0.0F, "Linear", reg_},
                  quadratic_ = {0.0F, "Quadratic", reg_}, cut_off_ = {20.0F, "Cutoff", reg_},
                  outer_cut_off_ = {25.0F, "Outer Cutoff", reg_};
  void Init() override;
  void Quit() override;
  void Load(pugi::xml_node&) override;
  [[nodiscard]] std::string GetTypeName() const override {
    return "SpotLight";
  }
  void Save(pugi::xml_node&) const override;
  void SetUniforms(const std::unique_ptr<Shader>&, int);
};

#endif  // LIGHT_H
