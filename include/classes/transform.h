#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <classes/component.h>
#include <classes/editable.h>
#include <glm/glm.hpp>

class Transform : public Component {
public:
  Editable<glm::vec3> position_ = {glm::vec3(0.0F), "Position", reg_};
  Editable<glm::vec2> scale_ = {glm::vec2(1.0F), "Scale", reg_};
  Editable<glm::vec3> rotation_ = {glm::vec3(0.0F), "Rotation", reg_};
  void Load(pugi::xml_node& /*unused*/) override;
  void Save(pugi::xml_node& /*unused*/) const override;
  ~Transform() = default;
  [[nodiscard]] const char* GetTypeName() const override {
    return "Transform";
  }
};

#endif  // TRANSFORM_H