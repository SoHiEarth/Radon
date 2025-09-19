#ifndef OBJECT_H
#define OBJECT_H

#include <box2d/box2d.h>
#include <classes/editable.h>
#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <string>

class Material;
class Object {
public:
  std::vector<IEditable*> reg_;
  Editable<std::string> name_ = {"Object", "Name", reg_};
  Editable<glm::vec3> position_ = {glm::vec3(0.0F), "Position", reg_};
  Editable<glm::vec2> scale_ = {glm::vec2(1.0F), "Scale", reg_};
  Editable<glm::vec3> rotation_ = {glm::vec3(0.0F), "Rotation", reg_};
  Editable<bool> is_static_ = {false, "Static", reg_};
  b2BodyId physics_body_;
  Material* material_ = nullptr;
  bool has_initialized_ = false;
  virtual void Init() {};
  virtual void Update() {};
  virtual void Render() {};
  bool has_quit_ = false;
  virtual void Quit() {};
  virtual void Load(pugi::xml_node&) = 0;
  [[nodiscard]] virtual std::string GetTypeName() const = 0;
  virtual void Save(pugi::xml_node&) const = 0;
  virtual ~Object() = default;
};

#endif  // OBJECT_H
