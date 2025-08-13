#ifndef OBJECT_H
#define OBJECT_H

#include <classes/editable.h>
#include <glm/glm.hpp>
#include <string>
class Material;
class Object {
public:
  std::vector<IEditable*> reg_;
  Editable<std::string> name_ = {"Object", "Name", reg_};
  Editable<glm::vec3> position_ = {glm::vec3(0.0F), "Position", reg_};
  Editable<glm::vec2> scale_ = {glm::vec2(1.0F), "Scale", reg_};
  Editable<float> rotation_ = {0.0F, "Rotation", reg_};
  Editable<bool> is_static_ = {false, "Static", reg_};
  Material* material_ = nullptr;
  virtual void Init() = 0;
  virtual void Update() = 0;
  virtual void Render() = 0;
  virtual void Quit() = 0;
};

#endif  // OBJECT_H
